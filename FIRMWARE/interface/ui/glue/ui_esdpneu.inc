#MP Macro ESDPneu_RunInfo
#MP If FEATURE_ESD_CONTINUOUS_DIAGS == FEATURE_ESD_CONTINUOUS_DIAGS_SUPPORTED
    #MP Expand DeclareStartNode(#@UINODEID_PNEUMATIC_TEST_RUNNING#)
    #MP Expand StartNode(nodetype_nav, #@ui_pneu_run#)
    { //Just display
        #MP Expand NodetType()
        -TMOUT_TICKS_REFRESH,
        #MP TEXTLABEL(PNEU_RUNNING)
        ENOFFS(EN_PNEU_ON),
        NODE_LINK(canceled_pneu), //proceed to analyze the outcome
        NODE_LINK(ui_pneu_run),  //pushbuttons will have us stuck here
        XLATE_INDEX(xlate_SelectIfEnabled), //To allow falling through the cycling menu in OOS
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, "canceled_pneu")
    {
        #MP Expand NodetType()
        TMOUT_TICKS_CANCELED,
        #MP TEXTLABEL(MSG_CANCELED)
        ENOFFS(EN_PNEU_CANCELED),
        NODE_LINK(#mp%sCyclingMenuStart), //do it on any key
        NODE_LINK(#mp%sCyclingMenuStart), //do it on disabled node
        XLATE_INDEX(xlate_NextIfEnabled), //To allow falling through the cycling menu in OOS
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP Endif ;FEATURE_ESD_CONTINUOUS_DIAGS == FEATURE_ESD_CONTINUOUS_DIAGS_SUPPORTED
#MP Endm
