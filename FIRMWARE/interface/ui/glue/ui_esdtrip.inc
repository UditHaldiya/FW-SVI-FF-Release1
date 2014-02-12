    #MP ;;;Expand CycleMenu_TrippedOrFailed(CyclingMenuNodeAfterSignal) ;Automatic next node will be taken

#MP ;---- An info sequence in the cycling menu ---------
#MP Macro CycleMenu_TrippedOrFailed ;(node_name[, node_link])
#MP If FEATURE_UI_SHOW_TRIPPED == FEATURE_UI_SHOW_TRIPPED_ENABLED
    #MP Ifdef start_failhigh_count_%dpass {NUM}
    #MP     start_failhigh_count_%dpass = start_failhigh_count_%dpass + 1
    #MP Else
    #MP     start_failhigh_count_%dpass = 0
    #MP Endif
#MP If FAULTS_LATCH_IS_FAILSAFE == OPTION_ON
    #MP Expand StartNode(nodetype_nav, #1#)
    {
        #MP Expand NodetType()
        TMOUT_TICKS_DEFAULT, //timeout
        #MP TEXTLABEL(IPTRIPPED)
        ENOFFS(iptripped), // enabled in non-OOS when tripped
        NODE_LINK(#mp%sMainMenuFirstNode),
        NODE_LINK(start_failhigh_#mp%dstart_failhigh_count_%dpass),
        XLATE_INDEX(xlate_OperMenu1),
        .uid = UNITSID_NONE
    },
    #MP Expand EndNode()
#MP Else
    #MP Setstr latched_trip_node = {uJoin, #1#, "__latched__"}
    #MP Expand StartNode(nodetype_nav, #1#)
    {
        #MP Expand NodetType()
        TMOUT_TICKS_DEFAULT, //timeout
        #MP TEXTLABEL(IPTRIPPED)
        ENOFFS(iptripped), // enabled in non-OOS when tripped
        NODE_LINK(#mp%sMainMenuFirstNode),
        NODE_LINK(#mp%slatched_trip_node),
        XLATE_INDEX(xlate_OperMenu1),
        .uid = UNITSID_NONE
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, latched_trip_node)
    {
        #MP Expand NodetType()
        TMOUT_TICKS_DEFAULT, //timeout
        #MP TEXTLABEL(FLBL_LATCH_FSAFE)
        ENOFFS(iptripped_latched), // enabled in non-OOS when tripped
        NODE_LINK(#mp%sMainMenuFirstNode),
        NODE_LINK(start_failhigh_#mp%dstart_failhigh_count_%dpass),
        XLATE_INDEX(xlate_OperMenu1),
        .uid = UNITSID_NONE
    },
    #MP Expand EndNode()
#MP Endif ;FAULTS_LATCH_IS_FAILSAFE == OPTION_OFF
    #MP nstart_failhigh_count_ = start_failhigh_count_%dpass ;get rid of composite name
    #MP Expand StartNode(nodetype_nav, {uJoin, "start_failhigh_", {%dnstart_failhigh_count_}})
    {
        #MP Expand NodetType()
        TMOUT_TICKS_DEFAULT, //timeout
        #MP TEXTLABEL(FAILED)
        ENOFFS(failhigh), // enabled in non-OOS when a failhigh fault is present
        NODE_LINK(#mp%sMainMenuFirstNode),
      #MP If #0#>1
        NODE_LINK(#mp%s#2#),
      #MP Else ;If link is not supplied, take the next node
        NODE_LINK(#mp%sUIGraphNodeNext),
      #MP Endif
        XLATE_INDEX(xlate_OperMenu1),
        .uid = UNITSID_NONE
    },
    #MP Expand EndNode()
#MP Endif ;FEATURE_UI_SHOW_TRIPPED == FEATURE_UI_SHOW_TRIPPED_ENABLED
#MP Endm
