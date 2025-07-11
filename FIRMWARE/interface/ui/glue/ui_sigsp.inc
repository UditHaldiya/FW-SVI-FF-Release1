#MP Macro CyclingSignal
    #MP Expand StartNode(nodetype_nav, #@start_sig#)
    {
        #MP Expand NodetType()
        TMOUT_TICKS_DEFAULT, //timeout
        #MP TEXTLABEL(SIG)
        ENOFFS(OPMODE_OPERMAN), // enabled in non-OOS
        NODE_LINK(#mp%sMainMenuFirstNode),
        NODE_LINK(var_start_sig),
        XLATE_INDEX(xlate_OperMenu1),
#MP If FEATURE_UI_CYCLING_SIGNAL_RES == FEATURE_UI_CYCLING_SIGNAL_RES_POINT3
        .uid = UNITSID_SIGNAL_ENTRY
#MP Endif
#MP If FEATURE_UI_CYCLING_SIGNAL_RES == FEATURE_UI_CYCLING_SIGNAL_RES_POINT1
        .uid = UNITSID_SIGNAL
#MP Endif
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_var, #@var_start_sig#)
    {
        #MP Expand NodetType()
        .checksum_offset = 0, //no checksum
        .en = UINODE_DISABLED,
        .parent = NODE_LINK(#mp%sUIGraphNodeNext),
        .data_index = 0,
        .varfunc = VARFUNC(ui_GetSignal)
    },
    #MP Expand EndNode()
#MP Endm
