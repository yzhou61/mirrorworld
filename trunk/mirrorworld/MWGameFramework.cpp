//////////////////////////////////////////////////////////////////////////
//
//
//
//
//////////////////////////////////////////////////////////////////////////
#include "MWGameFramework.h"

template<> MirrorWorld::GameFramework* Ogre::Singleton<MirrorWorld::GameFramework>::ms_Singleton = NULL;

namespace MirrorWorld {
//////////////////////////////////////////////////////////////////////////
// Constuctor
//////////////////////////////////////////////////////////////////////////
GameFramework::GameFramework()
{
    m_iNumScreenShots	= 0;

    m_pRoot = NULL;
    m_pRenderWnd = NULL;
    m_pViewport = NULL;
    m_pLog = NULL;
    m_pTimer = NULL;

    m_pInputMgr = NULL;
    m_pKeyboard = NULL;
    m_pMouse = NULL;

    m_pDebugOverlay = NULL;
}

//////////////////////////////////////////////////////////////////////////
// Deconstuctor
//////////////////////////////////////////////////////////////////////////
GameFramework::~GameFramework()
{
    if(m_pInputMgr)
        OIS::InputManager::destroyInputSystem(m_pInputMgr);

    delete m_pRoot;
}

//////////////////////////////////////////////////////////////////////////
// function initOgre
//////////////////////////////////////////////////////////////////////////
bool GameFramework::init(Ogre::String wndTitle, OIS::KeyListener *pKeyListener, OIS::MouseListener *pMouseListener)
{
    Ogre::LogManager* logMgr = new Ogre::LogManager();

    m_pLog = Ogre::LogManager::getSingleton().createLog("OgreLogfile.log", true, false, false);
    
#if defined(_DEBUG)
    m_pLog->setDebugOutputEnabled(true);
    m_pRoot = new Ogre::Root("plugins_d.cfg");
#else
    m_pRoot = new Ogre::Root("plugins.cfg");
#endif

    if (!m_pRoot->restoreConfig())
        if (!m_pRoot->showConfigDialog())
            return false;
    m_pRoot->saveConfig();

    m_pRenderWnd = m_pRoot->initialise(true, wndTitle);
    
    m_pViewport = m_pRenderWnd->addViewport(0);
    m_pViewport->setBackgroundColour(Ogre::ColourValue(0.8, 0.8, 0.8, 1.0));

    m_pViewport->setCamera(0);

    unsigned long hWnd = 0;
    OIS::ParamList paramList;
    m_pRenderWnd->getCustomAttribute("WINDOW", &hWnd);
    paramList.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWnd)));

    m_pInputMgr = OIS::InputManager::createInputSystem(paramList);

    m_pKeyboard = static_cast<OIS::Keyboard*>(m_pInputMgr->createInputObject(OIS::OISKeyboard, true));
    m_pMouse = static_cast<OIS::Mouse*>(m_pInputMgr->createInputObject(OIS::OISMouse, true));

    m_pMouse->getMouseState().height = m_pRenderWnd->getHeight();
    m_pMouse->getMouseState().width	 = m_pRenderWnd->getWidth();

    if (!pKeyListener)
        m_pKeyboard->setEventCallback(this);
    else
        m_pKeyboard->setEventCallback(pKeyListener);

    if (!pMouseListener)
        m_pMouse->setEventCallback(this);
    else
        m_pMouse->setEventCallback(pMouseListener);

    Ogre::String secName, typeName, archName;
    Ogre::ConfigFile cf;
    // TO DO: add resources.cfg
    cf.load("resources.cfg");

    Ogre::ConfigFile::SectionIterator iSec = cf.getSectionIterator();
    while (iSec.hasMoreElements())
    {
        secName = iSec.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = iSec.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
        }
    }
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    m_pTimer = new Ogre::Timer();
    m_pTimer->reset();

    m_pLog->logMessage("initing CEGUI...");
    m_pGUIRenderer = &CEGUI::OgreRenderer::bootstrapSystem(*m_pRenderWnd);
    m_pLog->logMessage("initing CEGUI Fin");

    // TO DO: add files needed by CEGUI
    CEGUI::SchemeManager::getSingleton().create((CEGUI::utf8*)"TaharezLookSkin.scheme"); 
    CEGUI::WindowManager::getSingleton().loadWindowLayout((CEGUI::utf8*)"GameFramework.layout"); 

    // Included in OgreCore.zip
    m_pDebugOverlay = Ogre::OverlayManager::getSingleton().getByName("Core/DebugOverlay");
    m_pDebugOverlay->show();

    m_pRenderWnd->setActive(true);
    return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool GameFramework::keyPressed(const OIS::KeyEvent &keyEventRef)
{
    if (m_pKeyboard->isKeyDown(OIS::KC_SYSRQ))
    {
        std::ostringstream ss;
        ss << "screenshot_" << ++m_iNumScreenShots << ".png";
        m_pRenderWnd->writeContentsToFile(ss.str());
        return true;
    }

    if (m_pKeyboard->isKeyDown(OIS::KC_O))
    {
        if (m_pDebugOverlay)
        {
            if (!m_pDebugOverlay->isVisible())
                m_pDebugOverlay->show();
            else
                m_pDebugOverlay->hide();
        }
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool GameFramework::keyReleased(const OIS::KeyEvent &keyEventRef)
{
    return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool GameFramework::mouseMoved(const OIS::MouseEvent &evt)
{
    return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool GameFramework::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    return true;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
bool GameFramework::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void GameFramework::updateOgre(double timeSinceLastFrame)
{
    updateStats();
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void GameFramework::updateStats() 
{ 
    static Ogre::String currFps = "Current FPS: "; 
    static Ogre::String avgFps = "Average FPS: "; 
    static Ogre::String bestFps = "Best FPS: "; 
    static Ogre::String worstFps = "Worst FPS: "; 
    static Ogre::String tris = "Triangle Count: "; 
    static Ogre::String batches = "Batch Count: "; 

    Ogre::OverlayElement* guiCurr = Ogre::OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");
    Ogre::OverlayElement* guiAvg = Ogre::OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
    Ogre::OverlayElement* guiBest = Ogre::OverlayManager::getSingleton().getOverlayElement("Core/BestFps"); 
    Ogre::OverlayElement* guiWorst = Ogre::OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");

    const Ogre::RenderTarget::FrameStats& stats = m_pRenderWnd->getStatistics(); 
    guiCurr->setCaption(currFps + Ogre::StringConverter::toString(stats.lastFPS)); 

    guiAvg->setCaption(m_DebugInfo[0]);
    guiBest->setCaption(m_DebugInfo[1]);
    guiWorst->setCaption(m_DebugInfo[2]); 

    Ogre::OverlayElement* guiTris = Ogre::OverlayManager::getSingleton().getOverlayElement("Core/NumTris"); 
    guiTris->setCaption(m_DebugInfo[3]);

    Ogre::OverlayElement* guiBatches = Ogre::OverlayManager::getSingleton().getOverlayElement("Core/NumBatches"); 
    guiBatches->setCaption(m_DebugInfo[4]); 

    Ogre::OverlayElement* guiDbg = Ogre::OverlayManager::getSingleton().getOverlayElement("Core/DebugText"); 
    guiDbg->setCaption("");
} 
}