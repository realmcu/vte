/*================================================================================================*/
/**
    @file   
    @brief  LTP Motorola template.
*/            
/*==================================================================================================

  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
     
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Alexey P           17/05/2004      ?????????   Initial version 

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/
#include "pencapmainwindowimpl.h"


PenCapMainWindowImpl::PenCapMainWindowImpl( QWidget* parent, const char* name, WFlags f )
	: PenCapMainWindow( parent, name, f )
{
	setCaption("PenCap");

	// Add your code

}
