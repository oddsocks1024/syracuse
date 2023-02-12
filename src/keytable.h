/*
  Keyboard Tables*/

int keytable[512][2];

int keys[][3]=
{
        {KEY_A,3,12},{KEY_B,5,2}, {KEY_C,5,0}, {KEY_D,3,14},
        {KEY_E,2,9}, {KEY_F,3,15},{KEY_G,4,0}, {KEY_H,4,1},
        {KEY_I,2,14},{KEY_J,4,2}, {KEY_K,4,3}, {KEY_L,4,4},
        {KEY_M,5,4}, {KEY_N,5,3}, {KEY_O,2,15},{KEY_P,3,0},
        {KEY_Q,2,7}, {KEY_R,2,10},{KEY_S,3,13},{KEY_T,2,11},
        {KEY_U,2,13},{KEY_V,5,1}, {KEY_W,2,8}, {KEY_X,4,15},
        {KEY_Y,2,12},{KEY_Z,4,14},{KEY_0,1,10},{KEY_1,1,1},
        {KEY_2,1,2}, {KEY_3,1,3}, {KEY_4,1,4}, {KEY_5,1,5},
        {KEY_6,1,6},    {KEY_7,1,7},    {KEY_8,1,8},    {KEY_9,1,9},
        {KEY_0_PAD,6,5},{KEY_1_PAD,5,10},{KEY_2_PAD,5,11},{KEY_3_PAD,5,12},
        {KEY_4_PAD,4,8},{KEY_5_PAD,4,9},{KEY_6_PAD,4,10},{KEY_7_PAD,3,7},
        {KEY_8_PAD,3,8},{KEY_9_PAD,3,9},{KEY_F1,0,1},   {KEY_F2,0,2},
        {KEY_F3,0,3},{KEY_F4,0,4},{KEY_F5,0,5},{KEY_F6,0,6},
        {KEY_F7,0,7},{KEY_F8,0,8},{KEY_F9,0,9},{KEY_F10,0,10},
        {KEY_F11,0,11}, {KEY_F12,0,12}, {KEY_ESC,0,0}, {KEY_TILDE,1,0},
        {KEY_MINUS,1,11}, {KEY_EQUALS,1,12}, {KEY_BACKSPACE,1,14}, {KEY_TAB,2,6},
        {KEY_OPENBRACE,3,1}, {KEY_CLOSEBRACE,3,2}, {KEY_ENTER,4,7}, {KEY_SEMICOLON,4,5},
        {KEY_COLON,4,5},
        {KEY_QUOTE,4,6}, {KEY_SLASH,5,7}, {KEY_BACKSLASH,3,3}, {KEY_COMMA,5,5},
        {KEY_STOP,5,6}, {KEY_SLASH,5,7}, {KEY_SPACE,5,15}, {KEY_INSERT,1,15},
        {KEY_DEL,3,4}, {KEY_HOME,2,0}, {KEY_END,3,5}, {KEY_PGUP,2,1},
        {KEY_PGDN,3,6}, {KEY_LEFT,6,2}, {KEY_RIGHT,6,4}, {KEY_UP,5,9},
        {KEY_DOWN,6,3}, {KEY_SLASH_PAD,2,3}, {KEY_ASTERISK,2,4}, {KEY_MINUS_PAD,2,5},
        {KEY_PLUS_PAD,4,11},{KEY_LSHIFT,4,12},  {KEY_RSHIFT,5,8}, {KEY_LCONTROL,3,11},
        {KEY_RCONTROL,6,1}, {KEY_CAPSLOCK,5,13},{KEY_SCRLOCK,0,14},{KEY_NUMLOCK,2,2},
        {KEY_ALT,5,14},{KEY_ALTGR,6,0},{KEY_BACKSLASH2,3,3},{KEY_DEL_PAD,6,6},{KEY_ENTER_PAD,6,7},
        {KEY_PAUSE,0,15},{-1,-1,-1}
};

int keys_a500[][3]=
{
        {KEY_A,1,8}, {KEY_B,3,3}, {KEY_C,2,5}, {KEY_D,2,8},
        {KEY_E,1,2}, {KEY_F,2,9}, {KEY_G,2,3}, {KEY_H,3,8},
        {KEY_I,8,6}, {KEY_J,8,8}, {KEY_K,8,3}, {KEY_L,9,8},
        {KEY_M,8,5}, {KEY_N,3,5}, {KEY_O,8,9}, {KEY_P,9,6},
        {KEY_Q,1,6}, {KEY_R,2,7}, {KEY_S,1,9}, {KEY_T,2,6},
        {KEY_U,3,9}, {KEY_V,2,4}, {KEY_W,1,7}, {KEY_X,1,4},
        {KEY_Y,3,6}, {KEY_Z,1,3}, {KEY_0,9,2}, {KEY_1,0,6},
        {KEY_2,0,2}, {KEY_3,1,1}, {KEY_4,2,1}, {KEY_5,2,2},
        {KEY_6,3,2}, {KEY_7,3,7}, {KEY_8,8,2}, {KEY_9,8,7},
        {KEY_0_PAD,4,9}, {KEY_1_PAD,4,6}, {KEY_2_PAD,7,3}, {KEY_3_PAD,7,5},
        {KEY_4_PAD,4,2}, {KEY_5_PAD,7,9}, {KEY_6_PAD,7,6}, {KEY_7_PAD,4,1},
        {KEY_8_PAD,7,7}, {KEY_9_PAD,7,2}, {KEY_F1,0,1}, {KEY_F2,0,0},
        {KEY_F3,1,0}, {KEY_F4,2,0}, {KEY_F5,3,1}, {KEY_F6,3,0},
        {KEY_F7,8,1}, {KEY_F8,8,0}, {KEY_F9,9,1}, {KEY_F10,9,0},
        {KEY_F11,4,0}, {KEY_F12,7,1}, {KEY_ESC,5,0}, {KEY_TILDE,5,2},
        {KEY_MINUS,9,7}, {KEY_EQUALS,0,9}, /*{KEY_BACKSPACE,1,14},*/ {KEY_TAB,0,3},
        {KEY_OPENBRACE,5,6}, {KEY_CLOSEBRACE,5,8}, {KEY_ENTER,5,5}, {KEY_SEMICOLON,9,3},
        {KEY_COLON,9,3},
        {KEY_QUOTE,5,3}, {KEY_SLASH,9,4}, {KEY_BACKSLASH,1,5}, {KEY_COMMA,8,4},
        {KEY_STOP,9,5}, {KEY_SPACE,3,4}, /*{KEY_INSERT,1,15},*/
        {KEY_DEL,5,7}, /*{KEY_HOME,2,0}, */{KEY_END,0,8},/* {KEY_PGUP,2,1},
        {KEY_PGDN,3,6},*/ {KEY_LEFT,4,3}, {KEY_RIGHT,4,8}, {KEY_UP,4,7},
        {KEY_DOWN,4,3}, {KEY_SLASH_PAD,7,0}, {KEY_ASTERISK,6,0}, {KEY_MINUS_PAD,6,1},
        {KEY_PLUS_PAD,7,8}, {KEY_LSHIFT,10,0},  {KEY_RSHIFT,10,0}, {KEY_LCONTROL,12,0},
        {KEY_RCONTROL,12,0}, {KEY_CAPSLOCK,0,4}, /*{KEY_SCRLOCK,0,14},{KEY_NUMLOCK,2,2},
        {KEY_ALT,5,14},{KEY_ALTGR,6,0},*/{KEY_BACKSLASH2,1,5}, {KEY_DEL_PAD,4,4}, {KEY_ENTER_PAD,7,4},
        /*{KEY_PAUSE,0,15},*/{-1,-1,-1}
};
