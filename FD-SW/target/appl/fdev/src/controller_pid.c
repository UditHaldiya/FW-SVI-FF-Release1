/*
Copyright 2013 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file controller_pid.c. This is the PID algorithm.
    \brief PID algorithm and re-calculations

*/
/********************************************************************************
* Project    :  PID Controller
*
* Platform   :  STM32F101ZGT6
*
* Class/Type :  Algorithm
*
* Compiler   :  IAR Workbench C/C++ Compiler for ARM Version 6.3
*
* Date       :  2013/03
* Author     :  C.Heil, Mesco Engineering GmbH
*
*******************************************************************************/
/*
\verbatim
********************************************************************************
*  Changes
********************************************************************************
* Date        Sgn   Ver.  Changes
* 2013-03-26  ch    0.01  created
* 2013-10-31  ch    0.02  changed pid_check_convert_to_ideal()
**************************************************************** \endverbatim */


#include <base.h>
#include <fbap.h>
#include <ffbl_pid.h>
#include "defs_pid.h"
#ifndef _WIN32
    #include "osif.h"
#endif

#ifndef _WIN32
    #include <ffbl_int.h>
#endif


#include "controller_pid.h"

#define FLOATMINIMUM	((FLOAT)(0.001))

/*******************************************************************************
*** LOCAL functions
*******************************************************************************/

static BOOL pid_check_convert_to_ideal(T_PIDFB_DATA * p_pidfb_data, T_PIDFB * p_pidfb);
static BOOL pid_check_convert_to_series(T_PIDFB_DATA * p_pidfb_data, T_PIDFB * p_pidfb);
static BOOL pid_check_convert_to_parallel(T_PIDFB_DATA * p_pidfb_data, T_PIDFB * p_pidfb);
static BOOL pid_check_convert_to_i_pd(T_PIDFB_DATA * p_pidfb_data, T_PIDFB * p_pidfb);
static BOOL pid_check_convert_to_ip_d(T_PIDFB_DATA * p_pidfb_data, T_PIDFB * p_pidfb);
static void cond_bump(T_PIDFB_DATA * p_pidfb_data, const  T_PIDFB * p_pidfb);

static void pid_back_calc_para(const T_PIDFB_DATA * p_pidfb_data, T_PIDFB * p_pidfb);

/******************************************************************************/
/*! \brief   pid algorithm
*   \param   p_pidfb_data points to dynamic data
*   \param   p_pidfb points to the FF parameters
*   \param   p_control_data points to control data
*   \return  the calculated PID value
*
*   Function with algorithm to compute the pid controller
*******************************************************************************/
FLOAT pid_algo(T_PIDFB_DATA * p_pidfb_data, const  T_PIDFB * p_pidfb, T_PIDFB_CONTROL_DATA * p_control_data)
{
    FLOAT kd_val;
    FLOAT kp1_val;
    FLOAT kp2_val;
    FLOAT ki_val;
    FLOAT tfpd_val;
    FLOAT kpvl_val;
    FLOAT kpvc_val;
    FLOAT dtpv_val;
    FLOAT output_val;
    FLOAT sp_pv_deviation;
    FLOAT tfpid_val;

    /* Calculating output for tfpid */

    /* error = setpoint - process value */
    sp_pv_deviation = p_control_data->working_sp - p_control_data->working_pv;
	p_control_data->working_pv   *= -1.0f;

#ifndef _WIN32
    /* Apply control option 'direct acting' --------------------------------- */
    if (CNTRL_OPT_DIRECT_ACTING == (p_pidfb->control_opts & CNTRL_OPT_DIRECT_ACTING))
    {
        sp_pv_deviation              *= -1.0f;
        p_control_data->ff_deviation *= -1.0f;
		p_control_data->working_pv   *= -1.0f;
    }
#endif

    /* kp1 value = error * parameter kp1 */
    kp1_val = sp_pv_deviation * p_pidfb_data->kp_1;

    /* kd value = (kp1 value - old kp1 value) * parameter kd */
    kd_val = (kp1_val - (p_pidfb_data->kp_1 * p_pidfb_data->last_deviation)) *  p_pidfb_data->kd;

    /* kp2 value = kp1 value * parameter kp2 */
    kp2_val = kp1_val * p_pidfb_data->kp_2;

    /* tfpd value = (kd value + kp2 value) * parameter tfpd */
    tfpd_val = (kd_val + kp2_val);

    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

    /* Calculating output for kpvl */

    /* kpvl value = process value * parameter kpvl */
    kpvl_val = p_control_data->working_pv * p_pidfb_data->kpvl;

    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

    /* Calculating output for kpvc */

    /* dt process value = process value - last process value */
    dtpv_val = p_control_data->working_pv - p_pidfb_data->last_pv;

    /* kpvc value = dt process value * parameter kpvc */
    kpvc_val = dtpv_val * p_pidfb_data->kpvc;

    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

    /* Calculating the Integral term */
    /* ki value = kp1 value * parameter ki */
    ki_val = kp1_val * p_pidfb_data->ki;

    /* do not integrate when IN is Constant or Limited */
    if (NO_LIMIT == (LIMIT_MASK & p_pidfb->in.status))
    {
        /* isum value = last isum value + ki value */
        p_pidfb_data->I_sum = p_pidfb_data->I_sum + ki_val;
        p_pidfb_data->I_sum  += p_control_data->ff_deviation;/* ITK test failure, changed ch 20131209 */
    }

    /* Filter time */
    tfpid_val = ((((tfpd_val + p_pidfb_data->I_sum) - p_pidfb_data->last_tfpid) * p_pidfb_data->tfpid_factor) + p_pidfb_data->last_tfpid);

    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

    /* Calculating output */

    /* check whether the OUT value has not leaved the controllable region */

    output_val = tfpid_val + kpvl_val + kpvc_val;

    //AK added a hack
    if(p_pidfb->mode_blk.actual == MODE_IMAN)
    {
        //Propagate readback if we can
        if((p_pidfb->bkcal_in.status & SQ_GOOD) != 0U)
        {
            FLOAT new_output_val = p_pidfb->bkcal_in.value;
            FLOAT bump = output_val - new_output_val;
            if ((FLOAT)0.0 != p_pidfb_data->ki)/* bumpless transfer only useful with integral term */
            {
                p_pidfb_data->I_sum = p_pidfb_data->I_sum - bump;
                output_val = new_output_val;/* output shall not change */
            }
        }
    }

    /* was previous output value limited ? */
    if (output_val >= p_pidfb->out_hi_lim)
    {
        if (output_val >= p_pidfb_data->last_outputvalue)
        {
            p_pidfb_data->I_sum = p_pidfb_data->last_I_sum; /* undo the last addition */
            output_val = p_pidfb->out_hi_lim;
        }
    }
    if (output_val <= p_pidfb->out_lo_lim)
    {
        if (output_val <= p_pidfb_data->last_outputvalue)
        {
            p_pidfb_data->I_sum = p_pidfb_data->last_I_sum; /* undo the last addition */
            output_val = p_pidfb->out_lo_lim;
        }
    }

    /* must a bump less transfer provided ? */
    cond_bump(p_pidfb_data, p_pidfb);

    if (
        (CONDALGOCHG == (p_pidfb_data->bump_flag & CONDALGOCHG))/* has the algorithm changed ? */
        ||
        (CONDMODECHG == (p_pidfb_data->bump_flag & CONDMODECHG))/* has the block mode changed ? */
        )
    {    /* then a bump less transfer from last output to actual output must be provided */
        FLOAT bump = output_val - p_pidfb_data->last_outputvalue;
        if ((FLOAT)0.0 != p_pidfb_data->ki)/* bumpless transfer only useful with integral term */
        {
            p_pidfb_data->I_sum = p_pidfb_data->I_sum - bump;
            output_val = p_pidfb_data->last_outputvalue;/* output shall not change */
        }
    }

    if (CONDMANAUTO == (p_pidfb_data->bump_flag & CONDMANAUTO))/* has the mode switched from MANUAL to anything  else ? */
    {    /* then a bump less transfer from last Manual output to actual output must be provided */
        FLOAT bump = output_val - p_pidfb_data->out.value;
        if ((FLOAT)0.0 != p_pidfb_data->ki)/* bumpless transfer only useful with integral term */
        {
            p_pidfb_data->I_sum = p_pidfb_data->I_sum - bump;
            output_val = p_pidfb_data->out.value;/* output shall not change */
        }
    }

    if (CONDROUTAUTO == (p_pidfb_data->bump_flag & CONDROUTAUTO))/* has the mode switched from ROUT to anything  else ? */
    {    /* then a bump less transfer from last Rout output to actual output must be provided */
        FLOAT bump = output_val - p_pidfb->rout_in.value;
        if ((FLOAT)0.0 != p_pidfb_data->ki)/* bumpless transfer only useful with integral term */
        {
            p_pidfb_data->I_sum = p_pidfb_data->I_sum - bump;
            output_val = p_pidfb->rout_in.value;/* output shall not change */
        }
    }

    /* store some values for macro cycle delay */
    p_pidfb_data->last_deviation = sp_pv_deviation;

    p_pidfb_data->last_tfpid = tfpid_val;

    p_pidfb_data->last_pv = p_control_data->working_pv;

    p_pidfb_data->last_I_sum = p_pidfb_data->I_sum;

    p_pidfb_data->last_outputvalue = output_val;

    p_pidfb_data->last_new_target_mode = p_pidfb->mode_blk.actual;

    p_pidfb_data->bump_flag = CONDNO;

    return(output_val);
}



/******************************************************************************/
/*! \brief    PID prm
*   \param    p_pidfb_data points to dynamic data
*   \param    p_pidfb points to the FF parameters
*   \return    void
*
*   Function to re-calculating the pid parameters in case of switching the
*   parameter ALGORITHM. One division occurs, division by tfpid_factor.
*   This value is never zero. It's default value is 1.0.
*******************************************************************************/
void pid_prm(T_PIDFB_DATA * p_pidfb_data, T_PIDFB * p_pidfb)
{

    BOOL retval = FALSE;

    if (p_pidfb->algorithm != p_pidfb_data->last_algorithm)
    {
        p_pidfb_data->bump_flag |= CONDALGOCHG;/* signal the condition for bump less transfer */
    }

    switch(p_pidfb->algorithm)
    {
        case ALGO_SERIES:
        {
            retval = pid_check_convert_to_series(p_pidfb_data, p_pidfb);

            if(FALSE == retval) /* not able to convert, set defaults */
            {
                p_pidfb_data->kp_1    = p_pidfb_data->P_factor;
                p_pidfb_data->kp_2 = (FLOAT)1.0;
                p_pidfb_data->ki    = p_pidfb_data->I_factor;
                p_pidfb_data->kd    = p_pidfb_data->D_factor;
                p_pidfb->tfpid    = p_pidfb_data->period_of_exec / p_pidfb_data->tfpid_factor;
                p_pidfb_data->kpvl    = (FLOAT)0.0;
                p_pidfb_data->kpvc    = (FLOAT)0.0;
            }
            break;
        }
        case ALGO_PARALLEL:
        {
            retval = pid_check_convert_to_parallel(p_pidfb_data, p_pidfb);

            if(FALSE == retval)/* not able to convert, set defaults */
            {
                p_pidfb_data->kp_1    = (FLOAT)1.0;
                p_pidfb_data->kp_2    = p_pidfb_data->P_factor;
                p_pidfb_data->ki    = p_pidfb_data->I_factor;
                p_pidfb_data->kd    = p_pidfb_data->D_factor;
                p_pidfb->tfpid    = (FLOAT)PID_TFPID_OFF;
                p_pidfb_data->tfpid_factor = (FLOAT)PID_TF_FALSE;
                p_pidfb_data->kpvl    = (FLOAT)0.0;
                p_pidfb_data->kpvc    = (FLOAT)0.0;
            }
            break;
        }
        case ALGO_I_PD :
        {
            retval = pid_check_convert_to_i_pd(p_pidfb_data, p_pidfb);

            if (FALSE == retval)/* not able to convert, set defaults */
            {
                p_pidfb_data->kp_1    = p_pidfb_data->P_factor;
                p_pidfb_data->kp_2    = (FLOAT)0.0;
                p_pidfb_data->ki    = p_pidfb_data->I_factor;
                p_pidfb_data->kd    = (FLOAT)0.0;
                p_pidfb->tfpid    = (FLOAT)PID_TFPID_OFF;
                p_pidfb_data->tfpid_factor = (FLOAT)PID_TF_FALSE;
                p_pidfb_data->kpvl    = p_pidfb_data->P_factor;
                p_pidfb_data->kpvc    = p_pidfb_data->D_factor;
            }
            break;
        }
        case ALGO_IP_D :
        {
            retval = pid_check_convert_to_ip_d(p_pidfb_data, p_pidfb);

            if (FALSE == retval)/* not able to convert, set defaults */
            {
                p_pidfb_data->kp_1    = (FLOAT)1.0;
                p_pidfb_data->kp_2    = p_pidfb_data->P_factor;
                p_pidfb_data->ki    = p_pidfb_data->I_factor;
                p_pidfb_data->kd    = (FLOAT)0.0;
                p_pidfb->tfpid    = (FLOAT)PID_TFPID_OFF;
                p_pidfb_data->tfpid_factor = (FLOAT)PID_TF_FALSE;
                p_pidfb_data->kpvl    = (FLOAT)0.0;
                p_pidfb_data->kpvc    = p_pidfb_data->D_factor;
            }

            break;
        }
        case ALGO_IDEAL:
        default:/* fallthru */
        {
            if(ALGO_IDEAL != p_pidfb->algorithm)
            {
                p_pidfb->algorithm = ALGO_IDEAL;
            }

            retval = pid_check_convert_to_ideal(p_pidfb_data, p_pidfb);

            if(FALSE == retval)
            {
                p_pidfb_data->kp_1    = p_pidfb_data->P_factor;
                p_pidfb_data->kp_2    = (FLOAT)1.0;
                p_pidfb_data->ki    = p_pidfb_data->I_factor;
                p_pidfb_data->kd    = p_pidfb_data->D_factor;
                p_pidfb->tfpid    = (FLOAT)PID_TFPID_OFF;
                p_pidfb_data->tfpid_factor = (FLOAT)PID_TF_FALSE;
                p_pidfb_data->kpvl    = (FLOAT)0.0;
                p_pidfb_data->kpvc    = (FLOAT)0.0;
            }
            break;
        }
    }

}



/******************************************************************************/
/*! \brief   pid check convert to ideal
*   \param   p_pidfb_data points to dynamic data
*   \param   p_pidfb points to the FF parameters
*   \return  TRUE if re-calculation was possible
*
*   Function to convert parameter to ideal algorithm. The formulas used
*   depend from the actual algorithm.
*******************************************************************************/
static BOOL pid_check_convert_to_ideal(T_PIDFB_DATA * p_pidfb_data, T_PIDFB * p_pidfb)
{
    BOOL retval = TRUE;

    if(ALGO_PARALLEL == p_pidfb_data->last_algorithm)
    {
        /* Conversion from Parallel to Ideal */
        if (FLOATMINIMUM <= p_pidfb_data->kp_2)/* changed because of division V 0.02 */
        {
            p_pidfb_data->ki    = p_pidfb_data->ki / p_pidfb_data->kp_2;
            p_pidfb_data->kd    = p_pidfb_data->kd / p_pidfb_data->kp_2;
            p_pidfb_data->kp_1  = p_pidfb_data->kp_2;

            /* defaults for ideal */
            p_pidfb_data->kp_2 = (FLOAT)1.0;
            p_pidfb_data->kpvc = (FLOAT)0.0;
            p_pidfb_data->kpvl = (FLOAT)0.0;
            p_pidfb->tfpid    = (FLOAT)PID_TFPID_OFF;
            p_pidfb_data->tfpid_factor = (FLOAT)PID_TF_FALSE;

            /* back calculation to the fieldbus */
            p_pidfb_data->P_factor    = p_pidfb_data->kp_1;
            p_pidfb_data->I_factor    = p_pidfb_data->ki;
            p_pidfb_data->D_factor    = p_pidfb_data->kd;

            pid_back_calc_para(p_pidfb_data, p_pidfb);
        }
        else
        {
            retval = FALSE;
        }

    }
    else if(ALGO_I_PD == p_pidfb_data->last_algorithm)
    {
        if (FLOATMINIMUM <= p_pidfb_data->kpvl)
        {
            /* Conversion from I-PD to Ideal */
            p_pidfb_data->kp_1    = p_pidfb_data->kpvl;
            p_pidfb_data->ki    = p_pidfb_data->ki;
            p_pidfb_data->kd    = p_pidfb_data->kpvc / p_pidfb_data->kpvl;

            /* defaults for ideal */
            p_pidfb_data->kp_2 = (FLOAT)1.0;
            p_pidfb_data->kpvc = (FLOAT)0.0;
            p_pidfb_data->kpvl = (FLOAT)0.0;
            p_pidfb->tfpid    = (FLOAT)PID_TFPID_OFF;
            p_pidfb_data->tfpid_factor = (FLOAT)PID_TF_FALSE;

            /* back calculation to the fieldbus */
            p_pidfb_data->P_factor    = p_pidfb_data->kp_1;
            p_pidfb_data->I_factor    = p_pidfb_data->ki;
            p_pidfb_data->D_factor    = p_pidfb_data->kd;

            pid_back_calc_para(p_pidfb_data, p_pidfb);
        }
        else
        {
            retval = FALSE;
        }

    }
    else if(ALGO_IP_D == p_pidfb_data->last_algorithm)
    {
        if (FLOATMINIMUM <= p_pidfb_data->kp_2)
        {
            /* Conversion from IP-D to Ideal */
            p_pidfb_data->kp_1  = p_pidfb_data->kp_2;
            p_pidfb_data->ki    = p_pidfb_data->ki / p_pidfb_data->kp_2;
            p_pidfb_data->kd    = p_pidfb_data->kpvc / p_pidfb_data->kp_2;

            /* defaults for ideal */
            p_pidfb_data->kp_2 = (FLOAT)1.0;
            p_pidfb_data->kpvc = (FLOAT)0.0;
            p_pidfb_data->kpvl = (FLOAT)0.0;
            p_pidfb->tfpid    = (FLOAT)PID_TFPID_OFF;
            p_pidfb_data->tfpid_factor = (FLOAT)PID_TF_FALSE;

            /* back calculation to the fieldbus */
            p_pidfb_data->P_factor    = p_pidfb_data->kp_1;
            p_pidfb_data->I_factor    = p_pidfb_data->ki;
            p_pidfb_data->D_factor    = p_pidfb_data->kd;

            /* formula is delivered in addition */
            pid_back_calc_para(p_pidfb_data, p_pidfb);
        }
        else
        {
            retval = FALSE;
        }
    }
    else if(ALGO_SERIES == p_pidfb_data->last_algorithm)
    {
        /* Conversion from Series to Ideal */
        p_pidfb_data->kp_1  = p_pidfb_data->kp_1;
        p_pidfb_data->ki    = p_pidfb_data->ki;
        p_pidfb_data->kd    = p_pidfb_data->kd;

        /* defaults for series */
        p_pidfb_data->kp_2 = (FLOAT)1.0;/* assuming this */
        p_pidfb_data->kpvc = (FLOAT)0.0;
        p_pidfb_data->kpvl = (FLOAT)0.0;
        p_pidfb->tfpid    = (FLOAT)PID_TFPID_OFF;
        p_pidfb_data->tfpid_factor = (FLOAT)PID_TF_FALSE;

        /* back calculation to the fieldbus */
        p_pidfb_data->P_factor    = p_pidfb_data->kp_1;
        p_pidfb_data->I_factor    = p_pidfb_data->ki;
        p_pidfb_data->D_factor    = p_pidfb_data->kd;

        pid_back_calc_para(p_pidfb_data, p_pidfb);
    }
    else
    {
        retval = FALSE;
    }

     /* set new algorithm to last */
    p_pidfb_data->last_algorithm = p_pidfb->algorithm;

    return (retval);
}


/******************************************************************************/
/*! \brief   pid check convert to series
*   \param   p_pidfb_data points to dynamic data
*   \param   p_pidfb points to the FF parameters
*   \return  TRUE if re-calculation was possible
*
*   Function to convert parameter to series algorithm. The formulas used
*   depend from the actual algorithm.
*******************************************************************************/
static BOOL pid_check_convert_to_series(T_PIDFB_DATA * p_pidfb_data, T_PIDFB * p_pidfb)
{
    BOOL retval = TRUE;

    if(ALGO_PARALLEL == p_pidfb_data->last_algorithm)
    {
        if (FLOATMINIMUM <= p_pidfb_data->kp_2)
        {

            /* back calculation to the fieldbus */
            /* Conversion from Parallel to Serial */
            p_pidfb_data->ki    = p_pidfb_data->ki / p_pidfb_data->kp_2;
            p_pidfb_data->kd    = p_pidfb_data->kd / p_pidfb_data->kp_2;
            p_pidfb_data->kp_1  = p_pidfb_data->kp_2;

            /* defaults for series */
            p_pidfb_data->kp_2 = (FLOAT)1.0;
            p_pidfb_data->kpvc = (FLOAT)0.0;
            p_pidfb_data->kpvl = (FLOAT)0.0;

            /* back calculation to the fieldbus */
            p_pidfb_data->P_factor    = p_pidfb_data->kp_1;/* parallel GAIN --> Series GAIN */
            p_pidfb_data->I_factor    = p_pidfb_data->ki;
            p_pidfb_data->D_factor    = p_pidfb_data->kd;

            pid_back_calc_para(p_pidfb_data, p_pidfb);
        }
        else
        {
            retval = FALSE;
        }
    }
    else if (ALGO_IDEAL == p_pidfb_data->last_algorithm)
    {
        /* Conversion from Ideal to Serial */
        p_pidfb_data->kp_1  = p_pidfb_data->kp_1;
        p_pidfb_data->ki    = p_pidfb_data->ki;
        p_pidfb_data->kd    = p_pidfb_data->kd;

        /* defaults for series */
        p_pidfb_data->kp_2  = (FLOAT)1.0;
        p_pidfb_data->kpvc = (FLOAT)0.0;
        p_pidfb_data->kpvl = (FLOAT)0.0;

        /* back calculation to the fieldbus */
        p_pidfb_data->P_factor    = p_pidfb_data->kp_1;
        p_pidfb_data->I_factor    = p_pidfb_data->ki;
        p_pidfb_data->D_factor    = p_pidfb_data->kd;

        pid_back_calc_para(p_pidfb_data, p_pidfb);
    }
    else if(ALGO_IP_D == p_pidfb_data->last_algorithm)
    {
        if (FLOATMINIMUM <= p_pidfb_data->kp_2)
        {
            /* Conversion from IP-D to series */
            p_pidfb_data->kp_1 = p_pidfb_data->kp_2;
            p_pidfb_data->ki = p_pidfb_data->ki / p_pidfb_data->kp_2;
            p_pidfb_data->kd = p_pidfb_data->kpvc / p_pidfb_data->kp_2;

            /* defaults for serial */
            p_pidfb_data->kp_2 = (FLOAT)1.0;
            p_pidfb_data->kpvc = (FLOAT)0.0;
            p_pidfb_data->kpvl = (FLOAT)0.0;

            /* back calculation to the fieldbus */
            p_pidfb_data->P_factor = p_pidfb_data->kp_1;
            p_pidfb_data->I_factor = p_pidfb_data->ki;
            p_pidfb_data->D_factor = p_pidfb_data->kd;

            pid_back_calc_para(p_pidfb_data, p_pidfb);
        }
        else
        {
            retval = FALSE;
        }
    }
    else if(ALGO_I_PD == p_pidfb_data->last_algorithm)
    {
        if (FLOATMINIMUM <= p_pidfb_data->kpvl)
        {
			/* Conversion from I-PD to series */
            p_pidfb_data->kp_1    = p_pidfb_data->kpvl;
            p_pidfb_data->ki    = p_pidfb_data->ki;
            p_pidfb_data->kd    = p_pidfb_data->kpvc / p_pidfb_data->kpvl;

			/* defaults for series */
			p_pidfb_data->kp_2 = (FLOAT)1.0;
			p_pidfb_data->kpvl = (FLOAT)0.0;
			p_pidfb_data->kpvc = (FLOAT)0.0;

			/* back calculation to the fieldbus */
			p_pidfb_data->P_factor = p_pidfb_data->kp_1;
			p_pidfb_data->I_factor = p_pidfb_data->ki;
			p_pidfb_data->D_factor = p_pidfb_data->kd;

			pid_back_calc_para(p_pidfb_data, p_pidfb);
		}
        else
        {
            retval = FALSE;
        }
    }

    else
    {
        retval = FALSE;
    }

     /* set new algorithm to last */
    p_pidfb_data->last_algorithm = p_pidfb->algorithm;

    return (retval);
}


/******************************************************************************/
/*! \brief   pid check convert to parallel
*   \param   p_pidfb_data points to dynamic data
*   \param   p_pidfb points to the FF parameters
*   \return  TRUE if re-calculation was possible
*
*   Function to convert parameter to parallel algorithm. The formulas used
*   depend from the actual algorithm.
*******************************************************************************/
static BOOL pid_check_convert_to_parallel(T_PIDFB_DATA * p_pidfb_data, T_PIDFB * p_pidfb)
{
    BOOL retval = TRUE;

    if(ALGO_IDEAL == p_pidfb_data->last_algorithm)
    {
        /* Conversion from Ideal to Parallel */
        p_pidfb_data->kp_2 = p_pidfb_data->kp_1;
        p_pidfb_data->ki    = p_pidfb_data->ki * p_pidfb_data->kp_1;
        p_pidfb_data->kd    = p_pidfb_data->kd * p_pidfb_data->kp_1;

        /* defaults for parallel */
        p_pidfb_data->kp_1 = (FLOAT)1.0;
        p_pidfb_data->kpvc = (FLOAT)0.0;
        p_pidfb_data->kpvl = (FLOAT)0.0;
        p_pidfb->tfpid    = (FLOAT)PID_TFPID_OFF;
        p_pidfb_data->tfpid_factor = (FLOAT)PID_TF_FALSE;

        /* back calculation to the fieldbus */
        p_pidfb_data->P_factor = p_pidfb_data->kp_2;
        p_pidfb_data->I_factor = p_pidfb_data->ki;
        p_pidfb_data->D_factor = p_pidfb_data->kd;

        pid_back_calc_para(p_pidfb_data, p_pidfb);

    }
    else if(ALGO_IP_D == p_pidfb_data->last_algorithm)
    {
        /* Conversion from IP-D to Parallel */
        p_pidfb_data->kd = p_pidfb_data->kpvc;
        /* ki is equal ki */
        /* kp_2 is equal kp_2 */

        /* defaults for Parallel */
        p_pidfb_data->kp_1 = (FLOAT)1.0;
        p_pidfb_data->kpvl = (FLOAT)0.0;
        p_pidfb_data->kpvc = (FLOAT)0.0;
        p_pidfb->tfpid    = (FLOAT)PID_TFPID_OFF;
        p_pidfb_data->tfpid_factor = (FLOAT)PID_TF_FALSE;

        /* back calculation to the fieldbus */
        p_pidfb_data->P_factor = p_pidfb_data->kp_2;
        p_pidfb_data->I_factor = p_pidfb_data->ki;
        p_pidfb_data->D_factor = p_pidfb_data->kd;

        pid_back_calc_para(p_pidfb_data, p_pidfb);

    }
    else if(ALGO_I_PD == p_pidfb_data->last_algorithm)
    {
        /* Conversion from IP-D to Parallel */
        p_pidfb_data->kp_2 = p_pidfb_data->kpvl;
        p_pidfb_data->ki = p_pidfb_data->ki * p_pidfb_data->kp_1;
        p_pidfb_data->kd = p_pidfb_data->kpvc;

        /* defaults for Parallel */
        p_pidfb_data->kp_1 = (FLOAT)1.0;
        p_pidfb_data->kpvl = (FLOAT)0.0;
        p_pidfb_data->kpvc = (FLOAT)0.0;

        /* back calculation to the fieldbus */
        p_pidfb_data->P_factor = p_pidfb_data->kp_2;
        p_pidfb_data->I_factor = p_pidfb_data->ki;
        p_pidfb_data->D_factor = p_pidfb_data->kd;

        pid_back_calc_para(p_pidfb_data, p_pidfb);

    }
    else if(ALGO_SERIES == p_pidfb_data->last_algorithm)
    {
        /* Conversion from Series to Parallel */
        p_pidfb_data->kp_2 = p_pidfb_data->kp_1;
        p_pidfb_data->kd = p_pidfb_data->kd * p_pidfb_data->kp_1;
        p_pidfb_data->ki = p_pidfb_data->ki * p_pidfb_data->kp_1;

        /* defaults for Parallel */
        p_pidfb_data->kp_1 = (FLOAT)1.0;
        p_pidfb_data->kpvl = (FLOAT)0.0;
        p_pidfb_data->kpvc = (FLOAT)0.0;

        /* back calculation to the fieldbus */
        p_pidfb_data->P_factor = p_pidfb_data->kp_2;
        p_pidfb_data->I_factor = p_pidfb_data->ki;
        p_pidfb_data->D_factor = p_pidfb_data->kd;

        pid_back_calc_para(p_pidfb_data, p_pidfb);
    }
    else
    {
        retval = FALSE;
    }

     /* set new algorithm to last */
    p_pidfb_data->last_algorithm = p_pidfb->algorithm;

    return (retval);
}


/******************************************************************************/
/*! \brief   pid check convert to i-pd
*   \param   p_pidfb_data points to dynamic data
*   \param   p_pidfb points to the FF parameters
*   \return  TRUE if re-calculation was possible
*
*   Function to convert parameter to i-pd algorithm. The formulas used
*   depend from the actual algorithm.
*******************************************************************************/
static BOOL pid_check_convert_to_i_pd(T_PIDFB_DATA * p_pidfb_data, T_PIDFB * p_pidfb)
{
    BOOL retval = TRUE;

    if(ALGO_PARALLEL == p_pidfb_data->last_algorithm)
    {
        if (FLOATMINIMUM <= p_pidfb_data->kp_2)
		{
			/* Conversion from Parallel to I-PD */
			p_pidfb_data->kpvl = p_pidfb_data->kp_2;
			p_pidfb_data->kpvc = p_pidfb_data->kd;
			p_pidfb_data->ki = p_pidfb_data->ki / p_pidfb_data->kp_2;
			p_pidfb_data->kp_1 = p_pidfb_data->kp_2;

			/* defaults for I-PD */
			p_pidfb_data->kd = (FLOAT)0.0;
			p_pidfb_data->kp_2 = (FLOAT)0.0;
			p_pidfb->tfpid    = (FLOAT)PID_TFPID_OFF;
			p_pidfb_data->tfpid_factor = (FLOAT)PID_TF_FALSE;

			/* back calculation to the fieldbus */
			p_pidfb_data->P_factor = p_pidfb_data->kpvl;
			p_pidfb_data->I_factor = p_pidfb_data->ki;
			p_pidfb_data->D_factor = p_pidfb_data->kpvc;

			pid_back_calc_para(p_pidfb_data, p_pidfb);
		}
		else
		{
            retval = FALSE;
		}
    }
    else if(ALGO_IP_D == p_pidfb_data->last_algorithm)
    {
		if (FLOATMINIMUM <= p_pidfb_data->kp_2)
		{
			/* Conversion from IP-D to I-PD */
			p_pidfb_data->ki = p_pidfb_data->ki;
			/* kp_1 is equal kp_1 */
			p_pidfb_data->kpvl = p_pidfb_data->kp_2 * p_pidfb_data->kp_1;
			p_pidfb_data->kpvc = p_pidfb_data->kpvc / p_pidfb_data->kp_2;

			/* defaults for I-PD */
			p_pidfb_data->kp_2 = (FLOAT)0.0;
			p_pidfb_data->kd = (FLOAT)0.0;
			p_pidfb->tfpid    = (FLOAT)PID_TFPID_OFF;
			p_pidfb_data->tfpid_factor = (FLOAT)PID_TF_FALSE;

			/* back calculation to the fieldbus */
			p_pidfb_data->P_factor = p_pidfb_data->kpvl;
			p_pidfb_data->I_factor = p_pidfb_data->ki;
			p_pidfb_data->D_factor = p_pidfb_data->kpvc;

			pid_back_calc_para(p_pidfb_data, p_pidfb);
		}
		else
		{
            retval = FALSE;
		}

    }
    else if(ALGO_IDEAL == p_pidfb_data->last_algorithm)
    {
        /* Conversion from IDEAL to I-PD */
        /* kp_1 is equal kp_1 */
        /* ki is equal ki */
        p_pidfb_data->kpvl = p_pidfb_data->kp_1;
        p_pidfb_data->kpvc  = p_pidfb_data->kd * p_pidfb_data->kp_1;

        /* defaults for I-PD */
        p_pidfb_data->kp_2 = (FLOAT)0.0;
        p_pidfb_data->kd = (FLOAT)0.0;
        p_pidfb->tfpid    = (FLOAT)PID_TFPID_OFF;
        p_pidfb_data->tfpid_factor = (FLOAT)PID_TF_FALSE;

        /* back calculation to the fieldbus */
        p_pidfb_data->P_factor = p_pidfb_data->kpvl;
        p_pidfb_data->I_factor = p_pidfb_data->ki;
        p_pidfb_data->D_factor = p_pidfb_data->kpvc;

        pid_back_calc_para(p_pidfb_data, p_pidfb);

    }
    else if(ALGO_SERIES == p_pidfb_data->last_algorithm)
    {
        /* Conversion from Series to I-PD */
        /* ki is equal ki */
        /* kp_1 is equal kp_1 */
        p_pidfb_data->kpvl = p_pidfb_data->kp_1;
        p_pidfb_data->kpvc  = p_pidfb_data->kd * p_pidfb_data->kp_1;

        /* defaults for I-PD */
        p_pidfb_data->kp_2 = (FLOAT)0.0;
        p_pidfb_data->kd = (FLOAT)0.0;
        p_pidfb->tfpid    = (FLOAT)PID_TFPID_OFF;
        p_pidfb_data->tfpid_factor = (FLOAT)PID_TF_FALSE;

        /* back calculation to the fieldbus */
        p_pidfb_data->P_factor = p_pidfb_data->kpvl;
        p_pidfb_data->I_factor = p_pidfb_data->ki;
        p_pidfb_data->D_factor = p_pidfb_data->kpvc;

        pid_back_calc_para(p_pidfb_data, p_pidfb);

    }
    else
    {
        retval = FALSE;
    }

    /* set new algorithm to last */
    p_pidfb_data->last_algorithm = p_pidfb->algorithm;

    return (retval);
}


/******************************************************************************/
/*! \brief   pid check convert to ip-d
*   \param   p_pidfb_data points to dynamic data
*   \param   p_pidfb points to the FF parameters
*   \return  TRUE if re-calculation was possible
*
*   Function to convert parameter to ip-d algorithm. The formulas used
*   depend from the actual algorithm.
*******************************************************************************/
static BOOL pid_check_convert_to_ip_d(T_PIDFB_DATA * p_pidfb_data, T_PIDFB * p_pidfb)
{
    BOOL retval = TRUE;


    if(p_pidfb->algorithm != p_pidfb_data->last_algorithm)
    {
        if(ALGO_PARALLEL == p_pidfb_data->last_algorithm)
        {
            /* Conversion from Parallel to IP-D */
            p_pidfb_data->kpvc = p_pidfb_data->kd;
            /* ki is equal ki */
            /* kp_2 is equal kp_2 */

            /* defaults for IP-D */
            p_pidfb_data->kp_1 = (FLOAT)1.0;
            p_pidfb_data->kpvl = (FLOAT)0.0;
            p_pidfb_data->kd = (FLOAT)0.0;
            p_pidfb->tfpid    = (FLOAT)PID_TFPID_OFF;
            p_pidfb_data->tfpid_factor = (FLOAT)PID_TF_FALSE;

            /* back calculation to the fieldbus */
            p_pidfb_data->P_factor = p_pidfb_data->kp_2;
            p_pidfb_data->I_factor = p_pidfb_data->ki;
            p_pidfb_data->D_factor = p_pidfb_data->kpvc;

            pid_back_calc_para(p_pidfb_data, p_pidfb);

        }
        else if(ALGO_I_PD == p_pidfb_data->last_algorithm)
        {
            if (FLOATMINIMUM <= p_pidfb_data->kp_1)
            {
				/* Conversion from I-PD to IP-D */
				p_pidfb_data->ki  = p_pidfb_data->ki * p_pidfb_data->kp_1;
				p_pidfb_data->kp_2 = p_pidfb_data->kpvl / p_pidfb_data->kp_1;
				p_pidfb_data->kpvc = p_pidfb_data->kpvc;

				/* defaults for IP-D */
				p_pidfb_data->kp_1 = (FLOAT)1.0;
				p_pidfb_data->kpvl = (FLOAT)0.0;
				p_pidfb_data->kd = (FLOAT)0.0;
				p_pidfb->tfpid    = (FLOAT)PID_TFPID_OFF;
				p_pidfb_data->tfpid_factor = (FLOAT)PID_TF_FALSE;

				/* back calculation to the fieldbus */
				p_pidfb_data->P_factor = p_pidfb_data->kp_2;
				p_pidfb_data->I_factor = p_pidfb_data->ki;
				p_pidfb_data->D_factor = p_pidfb_data->kpvc;

				pid_back_calc_para(p_pidfb_data, p_pidfb);
			}
			else
			{
                retval = FALSE;
			}

        }
        else if(ALGO_IDEAL == p_pidfb_data->last_algorithm)
        {
            if (FLOATMINIMUM <= p_pidfb_data->kp_1)
            {
                /* Conversion from Ideal to IP-D */
                p_pidfb_data->ki = p_pidfb_data->ki * p_pidfb_data->kp_1;
                p_pidfb_data->kp_2 = p_pidfb_data->kp_1;
                p_pidfb_data->kpvc = p_pidfb_data->kd * p_pidfb_data->kp_1;

                /* defaults for IP-D */
                p_pidfb_data->kp_1 = (FLOAT)1.0;
                p_pidfb_data->kpvl = (FLOAT)0.0;
                p_pidfb_data->kd = (FLOAT)0.0;
                p_pidfb->tfpid    = (FLOAT)PID_TFPID_OFF;
                p_pidfb_data->tfpid_factor = (FLOAT)PID_TF_FALSE;

                /* back calculation to the fieldbus */
                p_pidfb_data->P_factor = p_pidfb_data->kp_2;
                p_pidfb_data->I_factor = p_pidfb_data->ki;
                p_pidfb_data->D_factor = p_pidfb_data->kpvc;

                pid_back_calc_para(p_pidfb_data, p_pidfb);
            }
            else
            {
                retval = FALSE;
            }
        }
        else if(ALGO_SERIES == p_pidfb_data->last_algorithm)
        {
            if (FLOATMINIMUM <= p_pidfb_data->kp_1)
			{
				/* Conversion from series to IP-D */
				p_pidfb_data->kp_2 = p_pidfb_data->kp_1;
				p_pidfb_data->ki = p_pidfb_data->ki * p_pidfb_data->kp_1;
				p_pidfb_data->kpvc = p_pidfb_data->kd * p_pidfb_data->kp_1;

				/* defaults for IP-D */
				p_pidfb_data->kp_1 = (FLOAT)1.0;
				p_pidfb_data->kpvl = (FLOAT)0.0;
				p_pidfb_data->kd = (FLOAT)0.0;
				p_pidfb->tfpid    = (FLOAT)PID_TFPID_OFF;
				p_pidfb_data->tfpid_factor = (FLOAT)PID_TF_FALSE;

				/* back calculation to the fieldbus */
				p_pidfb_data->P_factor = p_pidfb_data->kp_2;
				p_pidfb_data->I_factor = p_pidfb_data->ki;
				p_pidfb_data->D_factor = p_pidfb_data->kpvc;

				/* formula is delivered in addition */
				pid_back_calc_para(p_pidfb_data, p_pidfb);
			}
			else
			{
                retval = FALSE;
			}
        }
        else
        {
            retval = FALSE;
        }
    }
    else
    {
        retval = FALSE;

    }

     /* set new algorithm to last */
    p_pidfb_data->last_algorithm = p_pidfb->algorithm;

    return (retval);
}


/******************************************************************************/
/*! \brief   pid back calculating parameters
*   \param   p_pidfb_data points to dynamic data
*   \param   p_pidfb points to the FF parameters
*   \return  void
*
*   Function to calculating the parameter back. This must be done in case of
*   switching the parameter ALGORITHM to another value.
*******************************************************************************/
static void pid_back_calc_para(const T_PIDFB_DATA * p_pidfb_data, T_PIDFB * p_pidfb)
{

#ifndef _WIN32
    osif_disable_all_tasks ();
#endif

    /* calculating the gain parameter new */
    p_pidfb->gain = p_pidfb_data->P_factor;

    /* check the parameters to avoid a division by zero */
    if((FLOAT)0.0 != p_pidfb_data->I_factor)
    {
        /* calculating the reset parameter new */
        p_pidfb->reset = p_pidfb_data->period_of_exec / p_pidfb_data->I_factor;
    }
    else
    {
        p_pidfb->reset = (FLOAT)0.0;
    }

    /* calculating the rate parameter new */
    p_pidfb->rate = p_pidfb_data->period_of_exec * p_pidfb_data->D_factor;

#ifndef _WIN32
    osif_enable_all_tasks ();
#endif
}




/******************************************************************************/
/*! \brief   checks whether the block mode has changed
*   \param   p_pidfb_data points to dynamic data
*   \param   p_pidfb points to dynamic data
*   \return  void
*
*   Function to calculating block mode change. p_pidfb_data->bump_flag
*   includes the related flag.
*******************************************************************************/
static void cond_bump(T_PIDFB_DATA * p_pidfb_data, const  T_PIDFB * p_pidfb)
{

    p_pidfb_data->bump_flag = CONDNO;

    if (p_pidfb_data->last_new_target_mode != p_pidfb->mode_blk.actual)
    {
        if (
            (p_pidfb_data->last_new_target_mode & (MODE_AUTO | MODE_CAS | MODE_RCAS | MODE_OS | MODE_IMAN | MODE_LO))
            !=
            (p_pidfb->mode_blk.actual & (MODE_AUTO | MODE_CAS | MODE_RCAS | MODE_IMAN | MODE_LO))
            )
        {
            p_pidfb_data->bump_flag |= CONDMODECHG;
        }

        if (MODE_MAN == p_pidfb_data->last_new_target_mode)
        {
            if (0 < (p_pidfb->mode_blk.actual & (MODE_AUTO | MODE_CAS | MODE_RCAS | MODE_IMAN | MODE_LO)))
            {
                p_pidfb_data->bump_flag |= CONDMANAUTO;
            }
        }

        if (MODE_ROUT == p_pidfb_data->last_new_target_mode)
        {
            if (0 < (p_pidfb->mode_blk.actual & (MODE_AUTO | MODE_CAS | MODE_RCAS)))
            {
                p_pidfb_data->bump_flag |= CONDROUTAUTO;
            }
        }

    }
}

/* This line marks the end of the source */


