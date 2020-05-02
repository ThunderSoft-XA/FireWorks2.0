//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: 
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <FrmPlatform.h>
#define GL_GLEXT_PROTOTYPES

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
/*旧版本使用头文件
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
*/
#include <FrmApplication.h>
#include <OpenGLES/FrmShader.h>
#include <sstream> 
#include <iostream>
#include <fstream>

//using namespace std;
//ofstream g_logfile;
//ofstream logfile;

#include "Scene.h"
#include <time.h>

#if defined(__linux__)
#include <stdio.h>
#include <unistd.h>
#endif

#include <iostream>

#include <stdlib.h>


//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "FireWork" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    g_strWindowTitle = strName;
    m_nWidth   = 1920;
    m_nHeight  = 1080;
    g_fAspectRatio   = (FLOAT)m_nWidth / (FLOAT)m_nHeight;
    m_pFont = NULL;
    m_FrameCount = 0;
    m_LastFPSDisplayTime = 0.0f;
    //g_logfile.open(LOG_FILE, ios::out|ios::app);
    //logfile.open(FILE, ios::out|ios::app);
    //g_logfile << "file has been opened!" << std::endl;
    FrmLogMessage("ERROR: This is an OpenGL program\n");
}


//--------------------------------------------------------------------------------------
// Name: DrawScene()
// Desc: DrawScene
//--------------------------------------------------------------------------------------
void CSample::DrawScene(double time)
{
    FrmLogMessage("ERROR: enter DrawScene\n");

    glDisable( GL_DEPTH_TEST );
    FireWorkDraw();
}

//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    FrmLogMessage("ERROR: enter Initialize\n");

    //Create the font

    m_pFont = new CFrmFontGLES();
    if( FALSE == m_pFont->Create( "Samples/Fonts/Tuffy12.pak" ) )
    {
        FrmLogMessage("ERROR: create m_pFont failed\n");
        return FALSE;
    }
    
    //Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Samples/Textures/Logo.pak" ) )
    {
        return FALSE;
    }

    //Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    //Setup the user interface
    if( FALSE == m_UserInterface.Initialize( m_pFont, g_strWindowTitle ) )
    {
        return FALSE;
    }

    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5, m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( (char *)"Press \200 for Help", 1.0f, -1.0f );

    srand(time(NULL));
    
    InitScene(1050, 1700);
    
    return TRUE;
}



//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Update()
{
    
    FrmLogMessage("ERROR: enter Update\n");

    // Process input
    UINT32 nButtons;
    UINT32 nPressedButtons;
    FrmGetInput( &m_Input, &nButtons, &nPressedButtons );

    // Toggle user interface0
    if( nPressedButtons & INPUT_KEY_0 )
    {
        m_UserInterface.AdvanceState();
    }
    return;

}


//--------------------------------------------------------------------------------------
// Name: BuildInfoString()
// Desc: 
//--------------------------------------------------------------------------------------
void CSample::BuildInfoString(char *pBuffer, int iSize, float fFrameRate)
{
    FrmSprintf( pBuffer, iSize, "FPS: %0.2f", fFrameRate);
}

//--------------------------------------------------------------------------------------
// Name: FlameLogMsg_int()
// Desc: 
//--------------------------------------------------------------------------------------
void CSample::FlameLogMsg_int(int Msg)
{
    CHAR logMsg[80];
    FrmSprintf( logMsg, 80, "FlameLogMsg_int: %d", Msg);
    FrmLogMessage((char*) " %s\n", logMsg);  
}

//--------------------------------------------------------------------------------------
// Name: FlameLogMsg_float()
// Desc: 
//--------------------------------------------------------------------------------------
void CSample::FlameLogMsg_float(float Msg)
{
    CHAR logMsg[80];
    FrmSprintf( logMsg, 80, "FlameLogMsg_float: %0.2f", Msg);
    FrmLogMessage((char*) " %s\n", logMsg);
}

//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
//#define WINAPI_FAMILY
VOID CSample::Render()
{
    FrmLogMessage("ERROR: enter Render\n");

    // Display
    DrawScene(FrmGetTime());
    
    // Update the timer
    m_Timer.MarkFrame();

    //Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
    
}

