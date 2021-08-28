#include "hardware/dma.h"

#include "oled.h"
#include "oled_font.h"
#include "oled.pio.h"
#include "keys_config.h"
#include "key_scan.h"
#include "hid.h"

#define SH1107_COMMAND_COL_ADDR_L(L)  (0x00 + (L))
#define SH1107_COMMAND_COL_ADDR_H(U)  (0x10 + (U))
#define SH1107_COMMAND_DISP_ONOFF(ON) (0xAE + (ON))
#define SH1107_COMMAND_PAGE_ADDR(A)   (0xB0 + (A))
#define SH1107_COMMAND_NOP            (0xE3)

#define SH1107_COLUMNS 128
#define SH1107_CHAR_COLS 16
#define SH1107_CHAR_ROWS 16

typedef struct oled_state
{
    PIO pio;
    uint sm;

    bool on_target;
    bool on;

    int sm_ctrl_chan;
    int cmd_chan;
    int data_seq_chan;
    int data_chan;

    /* When a key is pressed, the display cycles through 8 byte offsets of
    the character, the rotation per key is stored here.
    The rotation is acheived thanks for the DMA wrap feature when copying
    bytes from the character tiles to the OLED - the character can be copied
    at a byte offset and the DMA wraps round */
    uint8_t rotate_state[ROWS][COLS];

    bool changed;

    const uint64_t *character_pointer_buffer[SH1107_CHAR_ROWS][SH1107_CHAR_COLS];

} oled_state_t;

static oled_state_t state;

static void setup_dma(const uint16_t *control_sequence, uint control_sequence_count, const uint8_t *commands, uint commands_count);
static void oled_Redraw(void);
static void oled_Off(void);

// Data to send to the PIO sm_ctrl_chan to update the full display
const uint16_t sm_control_sequence_to_update_display[] =
{
    SM_CONTROL_HEADER(A_PIN_CMD, CMD_SM, 4),                // Commands to initialise display, Page 0
    SM_CONTROL_HEADER(A_PIN_DAT, DAT_SM, SH1107_COLUMNS),   // COL0 data
    SM_CONTROL_HEADER(A_PIN_CMD, CMD_SM, 1),                // Page 1
    SM_CONTROL_HEADER(A_PIN_DAT, DAT_SM, SH1107_COLUMNS),   // COL1 data
    SM_CONTROL_HEADER(A_PIN_CMD, CMD_SM, 1),                // Page 2
    SM_CONTROL_HEADER(A_PIN_DAT, DAT_SM, SH1107_COLUMNS),   // COL2 data
    SM_CONTROL_HEADER(A_PIN_CMD, CMD_SM, 1),                // Page 3
    SM_CONTROL_HEADER(A_PIN_DAT, DAT_SM, SH1107_COLUMNS),   // COL3 data
    SM_CONTROL_HEADER(A_PIN_CMD, CMD_SM, 1),                // Page 4
    SM_CONTROL_HEADER(A_PIN_DAT, DAT_SM, SH1107_COLUMNS),   // COL4 data
    SM_CONTROL_HEADER(A_PIN_CMD, CMD_SM, 1),                // Page 5
    SM_CONTROL_HEADER(A_PIN_DAT, DAT_SM, SH1107_COLUMNS),   // COL5 data
    SM_CONTROL_HEADER(A_PIN_CMD, CMD_SM, 1),                // Page 6
    SM_CONTROL_HEADER(A_PIN_DAT, DAT_SM, SH1107_COLUMNS),   // COL6 data
    SM_CONTROL_HEADER(A_PIN_CMD, CMD_SM, 1),                // Page 7
    SM_CONTROL_HEADER(A_PIN_DAT, DAT_SM, SH1107_COLUMNS),   // COL7 data
    SM_CONTROL_HEADER(A_PIN_CMD, CMD_SM, 1),                // Page 8
    SM_CONTROL_HEADER(A_PIN_DAT, DAT_SM, SH1107_COLUMNS),   // COL8 data
    SM_CONTROL_HEADER(A_PIN_CMD, CMD_SM, 1),                // Page 9
    SM_CONTROL_HEADER(A_PIN_DAT, DAT_SM, SH1107_COLUMNS),   // COL9 data
    SM_CONTROL_HEADER(A_PIN_CMD, CMD_SM, 1),                // Page 10
    SM_CONTROL_HEADER(A_PIN_DAT, DAT_SM, SH1107_COLUMNS),   // COL10 data
    SM_CONTROL_HEADER(A_PIN_CMD, CMD_SM, 1),                // Page 11
    SM_CONTROL_HEADER(A_PIN_DAT, DAT_SM, SH1107_COLUMNS),   // COL11 data
    SM_CONTROL_HEADER(A_PIN_CMD, CMD_SM, 1),                // Page 12
    SM_CONTROL_HEADER(A_PIN_DAT, DAT_SM, SH1107_COLUMNS),   // COL12 data
    SM_CONTROL_HEADER(A_PIN_CMD, CMD_SM, 1),                // Page 13
    SM_CONTROL_HEADER(A_PIN_DAT, DAT_SM, SH1107_COLUMNS),   // COL13 data
    SM_CONTROL_HEADER(A_PIN_CMD, CMD_SM, 1),                // Page 14
    SM_CONTROL_HEADER(A_PIN_DAT, DAT_SM, SH1107_COLUMNS),   // COL14 data
    SM_CONTROL_HEADER(A_PIN_CMD, CMD_SM, 1),                // Page 15
    SM_CONTROL_HEADER(A_PIN_DAT, DAT_SM, SH1107_COLUMNS),   // COL15 data
};

// Data to send to the PIO sm_ctrl_chan to turn off the display
const uint16_t sm_control_sequence_to_turn_off_display[] =
{
    SM_CONTROL_HEADER(A_PIN_CMD, CMD_SM, 1),                // Command turn off display
};

// Commands to send to the PIO cmd_chan to update the entire display
const uint8_t commands_to_update_display[] =
{
    // First command sequence to turn on display and zero the addresses
    SH1107_COMMAND_DISP_ONOFF(1), SH1107_COMMAND_COL_ADDR_L(0), SH1107_COMMAND_COL_ADDR_H(0), SH1107_COMMAND_PAGE_ADDR(0),
    // Following commands to update the page
    SH1107_COMMAND_PAGE_ADDR(1),
    SH1107_COMMAND_PAGE_ADDR(2),
    SH1107_COMMAND_PAGE_ADDR(3),
    SH1107_COMMAND_PAGE_ADDR(4),
    SH1107_COMMAND_PAGE_ADDR(5),
    SH1107_COMMAND_PAGE_ADDR(6),
    SH1107_COMMAND_PAGE_ADDR(7),
    SH1107_COMMAND_PAGE_ADDR(8),
    SH1107_COMMAND_PAGE_ADDR(9),
    SH1107_COMMAND_PAGE_ADDR(10),
    SH1107_COMMAND_PAGE_ADDR(11),
    SH1107_COMMAND_PAGE_ADDR(12),
    SH1107_COMMAND_PAGE_ADDR(13),
    SH1107_COMMAND_PAGE_ADDR(14),
    SH1107_COMMAND_PAGE_ADDR(15),
};

// Commands to send to the PIO cmd_chan to turn off the display
const uint8_t commands_to_turn_off_display[] =
{
    SH1107_COMMAND_DISP_ONOFF(0)
};

void Oled_Init(PIO pio)
{
    state.pio = pio;
    sh1107_pio_init(pio, 12, 13, 14, 15);

    // sm_ctrl_chan triggers the cmd and data PIO SMs
    int sm_ctrl_chan = dma_claim_unused_channel(true);
    // cmd_chan writes command data to the command PIO SM
    int cmd_chan = dma_claim_unused_channel(true);
    // data_seq_chan triggers the data_chan to write a sequence of source data to the data PIO SM
    int data_seq_chan = dma_claim_unused_channel(true);
    // Writes data to the data PIO SM - triggered by the sequencer
    int data_chan = dma_claim_unused_channel(true);
}

void Oled_Start(void)
{
    state.on_target = true;
}

void Oled_Stop(void)
{
    state.on_target = false;
}

bool Oled_Refresh(void)
{
    if (!dma_channel_is_busy(state.sm_ctrl_chan) &&
        !dma_channel_is_busy(state.cmd_chan) &&
        !dma_channel_is_busy(state.data_seq_chan) &&
        !dma_channel_is_busy(state.data_chan))
    {
        if (state.on_target)
        {
            oled_Redraw();
            state.on = true;
        }
        else
        {
            if (state.on)
            {
                oled_Off();
                state.on = false;
            }
        }
    }
}

static void setup_dma(const uint16_t *control_sequence, uint control_sequence_count, const uint8_t *commands, uint commands_count)
{
    dma_channel_config c;
    uint pio_tx_dreq = pio_get_index(state.pio) ? DREQ_PIO1_TX0 : DREQ_PIO0_TX0;

    // The sm control channel transfers a series of commands from a linear buffer
    // into the CONTROL_SM PIO TX FIFO then halts.
    c = dma_channel_get_default_config(state.sm_ctrl_chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_dreq(&c, pio_tx_dreq + CONTROL_SM);
    dma_channel_configure(state.sm_ctrl_chan, &c, &state.pio->txf[CONTROL_SM],
                          control_sequence, control_sequence_count, false);

    // The command channel transfers a series of commands from a linear buffer
    // into the CMD_SM PIO TX FIFO then halts.
    c = dma_channel_get_default_config(state.cmd_chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_dreq(&c, pio_tx_dreq + CMD_SM);
    dma_channel_configure(state.cmd_chan, &c, &state.pio->txf[CMD_SM],
                          commands, commands_count, false);

    // The data sequencer channel transfers 1 word into the data channel2's control
    // read address register, then halts
    c = dma_channel_get_default_config(state.data_seq_chan);
    dma_channel_configure(state.data_seq_chan, &c, &dma_hw->ch[state.data_chan].al3_read_addr_trig, state.character_pointer_buffer, 1, false);

    // The data channel is set up to write to the DAT_SM PIO FIFO (paced by the
    // PIO's TX FIFO request signal) and then chain to the control channel
    // once it completes. The control channel programs a new read address and
    // retriggers the data channel. It writes 8 bytes (one character) in each transfer
    c = dma_channel_get_default_config(state.data_chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_dreq(&c, pio_tx_dreq + DAT_SM);
    channel_config_set_ring(&c, false, 3); // RX buffer wrap at 64-bit/8-byte boundary
    // Trigger ctrl_chan when data_chan completes
    channel_config_set_chain_to(&c, state.data_seq_chan);
    dma_channel_configure(state.data_chan, &c, &state.pio->txf[DAT_SM], NULL, 8, false);
}

static void set_character(uint r, uint c, const uint64_t *character)
{
    const uint64_t **current = &state.character_pointer_buffer[r][c];
    if (*current != character)
        state.changed = true;
    *current = character;
}

static const key_event_t layer_names[NLAYERS][5] =
{
    [LAYER_BASE]  = {EVENT_B, EVENT_A, EVENT_S, EVENT_E,    EVENT_NONE},
    [LAYER_NAV]   = {EVENT_N, EVENT_A, EVENT_V, EVENT_NONE, EVENT_NONE},
    [LAYER_MOUSE] = {EVENT_M, EVENT_O, EVENT_U, EVENT_S,    EVENT_E},
    [LAYER_NUM]   = {EVENT_N, EVENT_U, EVENT_M, EVENT_NONE, EVENT_NONE},
    [LAYER_FUN]   = {EVENT_F, EVENT_U, EVENT_N, EVENT_NONE, EVENT_NONE},
    [LAYER_SYM]   = {EVENT_S, EVENT_Y, EVENT_M, EVENT_NONE, EVENT_NONE},
};

static void oled_Redraw(void)
{
    uint row_offset = 1;
    uint col_offset = 1;
    const uint64_t *character_set = Hid_IsShifted() ? character_set_shift : character_set;
    layer_t layer = KeyScan_GetCurrentLayer();

    state.changed = false;

    FOREACH_ROW(r)
    {
        FOREACH_COL(c)
        {
            const key_config_t *kconf = Keys_GetKeyConfig(layer, r, c);
            key_event_t event = kconf->on_press;
            const uint64_t *character = character_set + event;
            uint8_t rs = state.rotate_state[r][c];

            if (rs || Key_IsPressed(Keys_GetKey(r, c)))
            {
                character = (const uint64_t*)((const uint8_t*)character + rs);
                state.rotate_state[r][c] = ((rs + 1) % 8);
            }
            if (c == (COLS / 2))
            {
                // Offset right hand keys to right of display
                col_offset = 8;
            }
            set_character(row_offset+r, col_offset+c, character);
        }
    }

    for (uint c = 0; c < count_of(layer_names[0]); c++)
    {
        key_event_t e = layer_names[layer][c];
        set_character(15, c, &character_set_shift[e]);
    }

    set_character(15, 11, Hid_IsCapsLocked()   ? &character_set[EVENT_SPACE] : &character_set[EVENT_NONE]);
    set_character(15, 13, Hid_IsNumLocked()    ? &character_set[EVENT_SPACE] : &character_set[EVENT_NONE]);
    set_character(15, 15, Hid_IsScrollLocked() ? &character_set[EVENT_SPACE] : &character_set[EVENT_NONE]);

    if (state.changed)
    {
        uint32_t mask = (1 << state.sm_ctrl_chan) |
                        (1 << state.cmd_chan) |
                        (1 << state.data_seq_chan) |
                        (1 << state.data_chan);
        setup_dma(sm_control_sequence_to_update_display, count_of(sm_control_sequence_to_update_display),
                  commands_to_update_display, count_of(commands_to_update_display));
        dma_start_channel_mask(mask);
    }
}

static void oled_Off(void)
{
    uint32_t mask = (1 << state.sm_ctrl_chan) |
                    (1 << state.cmd_chan);
    setup_dma(sm_control_sequence_to_turn_off_display, count_of(sm_control_sequence_to_turn_off_display),
              commands_to_turn_off_display, count_of(commands_to_turn_off_display));
    dma_start_channel_mask(mask);  
}
