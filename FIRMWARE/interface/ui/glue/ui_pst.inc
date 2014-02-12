#MP Macro PST_RunInfo
    #MP Expand DeclareStartNode(#@UINODEID_PST_RUNNING#)
    #MP Expand StartNode(nodetype_nav, #@ui_pst_run#)
    { //Just display
        #MP Expand NodetType()
        -TMOUT_TICKS_REFRESH,
        #MP TEXTLABEL(PST_RUNNING)
        ENOFFS(PST_ON),
        NODE_LINK(canceled_pst), //when the node is no longer enabled, proceed to analyze the outcome
        NODE_LINK(ui_pst_run), //pushbuttons will have us stuck here
        XLATE_INDEX(xlate_SelectIfEnabled), //To allow falling through the cycling menu in OOS
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, "canceled_pst")
    {
        #MP Expand NodetType()
        TMOUT_TICKS_CANCELED,
        #MP TEXTLABEL(MSG_CANCELED)
        ENOFFS(EN_PST_CANCELED), //UINODE_ENABLED,
        NODE_LINK(#mp%CyclingMenuStart), //do it on any key
        NODE_LINK(#mp%CyclingMenuStart), //do it on disabled node
        XLATE_INDEX(xlate_NextIfEnabled), //To allow falling through the cycling menu in OOS
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP Endm

