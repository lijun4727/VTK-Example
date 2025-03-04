/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkViewImage2DFullCommand.cxx 1080 2009-02-18 13:27:02Z acanale $
Language:  C++
Author:    $Author: acanale $
Date:      $Date: 2009-02-18 14:27:02 +0100 (Wed, 18 Feb 2009) $
Version:   $Revision: 1080 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// version vtkRenderingAddOn
#include "vtkViewImage2DFullCommand.h"
#include "vtkViewImage2D.h"
#include "vtkInteractorStyleImage2D.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"


void vtkViewImage2DFullCommand::Execute(vtkObject*    caller, 
                                          unsigned long event, 
                                          void*         callData)
{

  if( !this->IV )
  {
    return;
  }
  
  if ( !this->IV->GetImage() )
  {
    return;
  }
  

  if ( !this->IV->GetRenderer() ||
       !this->IV->GetRenderWindow() ||
       !this->IV->GetRenderWindowInteractor())
  {
    return;
  }
  
  
  // Reset
  if (event == vtkCommand::ResetWindowLevelEvent)
  {
    this->IV->SyncResetWindowLevel();
    return;
  }
    
  // Start
  if (event == vtkCommand::StartWindowLevelEvent)
  {
    this->InitialWindow = this->IV->GetColorWindow();
    this->InitialLevel = this->IV->GetColorLevel();
    return;
  }

  if (event == vtkCommand::EndWindowLevelEvent)
  {
    return;
  }

  
  if (event == vtkCommand::KeyPressEvent)
  {
    vtkRenderWindowInteractor *rwi = this->IV->GetRenderWindowInteractor();

    int orientation = 0;
    int mode = 0;

    switch( rwi->GetKeyCode() )
    {
        case 'o':
        case 'O':
          orientation = this->IV->GetOrientation();
          orientation = (orientation+1)%3;
          this->IV->SetOrientation(orientation);
          this->IV->Render();
          return;
	  
        case 'i':
        case 'I':
          mode = this->IV->GetInterpolationMode();
          this->IV->SetInterpolationMode((++mode)%2);
          this->IV->Render();
          return;
          
        case 'c':
        case 'C':
      this->IV->SetOrientation(this->IV->GetOrientation());
      this->IV->Render();
      return;
      
    }
    
    return;
  }

  if (event == vtkCommand::WindowLevelEvent)
  {
    // Adjust the window level here
    vtkInteractorStyleImage *isi = 
      static_cast<vtkInteractorStyleImage *>(caller);

    if( !isi )
    {
      return;
    }
    
    int *size;
    size = this->IV->GetRenderWindow()->GetSize();
    double window = this->InitialWindow;
    double level = this->InitialLevel;
    
    // Compute normalized delta
    double dx = 4.0 * 
      (isi->GetWindowLevelCurrentPosition()[0] - 
       isi->GetWindowLevelStartPosition()[0]) / size[0];
    double dy = 4.0 * 
      (isi->GetWindowLevelStartPosition()[1] - 
       isi->GetWindowLevelCurrentPosition()[1]) / size[1];
    
    // Scale by current values
    if (fabs(window) > 0.01)
      dx = dx * window;
    else
      dx = dx * (window < 0 ? -0.01 : 0.01);

    if (fabs(level) > 0.01)
      dy = dy * level;
    else
      dy = dy * (level < 0 ? -0.01 : 0.01);
  
    // Abs so that direction does not flip
    if (window < 0.0) 
      dx = -1*dx;
    if (level < 0.0) 
      dy = -1*dy;
    
    // Compute new window level
    double newWindow = dx + window;
    double newLevel;
    newLevel = level - dy;
    
    // Stay away from zero and really
    if (fabs(newWindow) < 0.01)
      newWindow = 0.01 * (newWindow < 0 ? -1 : 1);

    if (fabs(newLevel) < 0.01)
      newLevel = 0.01 * (newLevel < 0 ? -1 : 1);
    
    this->IV->SyncSetWindow(newWindow);
    this->IV->SyncSetLevel(newLevel);
    this->IV->Render();

    return;
  }

}
