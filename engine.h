//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// engine.h


#ifndef _ENGINE_H
#define _ENGINE_H

#include "multithread.h"
#include "setting.h"
#include "msgstream.h"
#include <set>


#if 0 //defined(WIN32)
enum
{
    ///
    WM_APP_engineNotify = WM_APP + 110,
};
#endif


///
enum EngineNotify
{
    EngineNotify_shellExecute,                              ///<
    EngineNotify_loadSetting,                               ///<
    EngineNotify_showDlg,                                   ///<
    EngineNotify_helpMessage,                               ///<
    EngineNotify_setForegroundWindow,                       ///<
    EngineNotify_clearLog,                                  ///<
};


///
class Engine
{
private:
    enum
    {
        MAX_GENERATE_KEYBOARD_EVENTS_RECURSION_COUNT   = 64,    ///<
        MAX_KEYMAP_PREFIX_HISTORY                      = 64,    ///<
    };

    typedef Keymaps::KeymapPtrList      KeymapPtrList;      ///<

    /// focus of a thread
    class FocusOfThread
    {
    public:
        bool            m_isConsole;                        ///< is hwndFocus console ?
     #if 0 //defined(WIN32)
        DWORD           m_threadId;                         ///< thread id
        HWND            m_hwndFocus;                        ///< window that has focus on the thread
     #endif
        tstringi        m_className;                        ///< class name of hwndFocus
        tstringi        m_titleName;                        ///< title name of hwndFocus
        KeymapPtrList   m_keymaps;                          ///< keymaps

    public:
        ///
        FocusOfThread()
            : m_isConsole(false)
         #if 0 //defined(WIN32)
            , m_threadId(0)
            , m_hwndFocus(NULL)
         #endif
        { }
    };
    typedef std::map<uint32_t/*ThreadId*/, FocusOfThread>   FocusOfThreads;     ///<

    typedef std::list<uint32_t/*ThreadId*/>                 DetachedThreadIds;  ///<

    /// current status in generateKeyboardEvents
    class Current
    {
    public:
        const Keymap *                      m_keymap;       ///< current keymap
        ModifiedKey                         m_mkey;         ///< current processing key that user inputed
        Keymaps::KeymapPtrList::iterator    m_i;            ///< index in currentFocusOfThread-&gt;keymaps

    public:
        ///
        bool isPressed() const { return m_mkey.m_modifier.isOn(Modifier::Type_Down); }
    };

    friend class FunctionParam;

    /// part of keySeq
    enum Part
    {
        Part_all,                                           ///<
        Part_up,                                            ///<
        Part_down,                                          ///<
    };

    ///
    class EmacsEditKillLine
    {
        tstring m_buf;                                      ///< previous kill-line contents
    public:
        bool    m_doForceReset;                             ///<
    private:
     #if 0 //defined(WIN32)
        ///
        HGLOBAL makeNewKillLineBuf(const _TCHAR *i_data, int *i_retval);
     #elif defined(__linux__) || defined(__APPLE__)
     #elif defined(__APPLE__)
     #endif

    public:
        ///
        void reset() { m_buf.resize(0); }
        /** EmacsEditKillLineFunc.
           clear the contents of the clopboard
           at that time, confirm if it is the result of the previous kill-line
          */
        void    func();
        /// EmacsEditKillLinePred
        int     pred();
    };

 #if 0 //defined(WIN32)
    /// window positon for &amp;WindowHMaximize, &amp;WindowVMaximize
    class WindowPosition
    {
    public:
        ///
        enum Mode
        {
            Mode_normal,                        ///
            Mode_H,                             ///
            Mode_V,                             ///
            Mode_HV,                            ///
        };

    public:
        HWND    m_hwnd;                         ///
        RECT    m_rc;                           ///
        Mode    m_mode;                         ///

    public:
        ///
        WindowPosition(HWND i_hwnd, const RECT &i_rc, Mode i_mode)
            : m_hwnd(i_hwnd)
            , m_rc(i_rc)
            , m_mode(i_mode)
        { }
    };
    typedef std::list<WindowPosition>   WindowPositions;
    typedef std::list<HWND>             WindowsWithAlpha;       ///< windows for &amp;WindowSetAlpha
 #endif

    enum InterruptThreadReason
    {
        InterruptThreadReason_Terminate,
        InterruptThreadReason_Pause,
        InterruptThreadReason_Resume,
    };

private:
    CriticalSection             m_cs;                           ///< criticalSection

    // setting
 #if 0 //defined(WIN32)
    HWND                        m_hwndAssocWindow;              ///< associated window (we post message to it)
 #endif
    Setting *volatile           m_setting;                      ///< setting

    // engine thread state
 #if 0 //defined(WIN32)
    HANDLE                      m_device;                       ///< mayu device
 #elif defined(__linux__)
 #elif defined(__APPLE__)
    int                         m_device;                       ///< mayu device
 #endif
    tstring                     m_mayudVersion;                 ///< version of mayud.sys
    bool                        m_didMayuStartDevice;           ///< Did the mayu start the mayu-device ?
 #if 0 //defined(WIN32)
    HANDLE                      m_threadEvent;                  /**< 1. thread has been started
                                                                     2. thread is about to end*/
  #if defined(_WINNT)
    HANDLE                      m_readEvent;                    ///< reading from mayu device has been completed
    HANDLE                      m_interruptThreadEvent;         ///< interrupt thread event
    volatile InterruptThreadReason
                                m_interruptThreadReason;        ///< interrupt thread reason
    OVERLAPPED                  m_ol;                           ///< for async read/write of mayu device
    HANDLE                      m_hookPipe;                     ///< named pipe for &SetImeString
  #endif // _WINNT
 #elif defined(__linux__)
    //TODO:
 #elif defined(__APPLE__)
 #endif
    bool volatile               m_doForceTerminate;             ///< terminate engine thread
    bool volatile               m_isLogMode;                    ///< is logging mode ?
    bool volatile               m_isEnabled;                    ///< is enabled  ?
    bool volatile               m_isSynchronizing;              ///< is synchronizing ?
 #if 0 //defined(WIN32)
    HANDLE                      m_eSync;                        ///< event for synchronization
 #elif defined(__linux__)
    // TODO:
 #elif defined(__APPLE__)
 #endif
    int                         m_generateKeyboardEventsRecursionGuard; ///< guard against too many recursion

    // current key state
    Modifier                    m_currentLock;                  ///< current lock key's state
    int                         m_currentKeyPressCount;         ///< how many keys are pressed phisically ?
    int                         m_currentKeyPressCountOnWin32;  ///< how many keys are pressed on win32 ?
    Key *                       m_lastGeneratedKey;             ///< last generated key
    Key *                       m_lastPressedKey[2];            ///< last pressed key
    ModifiedKey                 m_oneShotKey;                   ///< one shot key
    unsigned int                m_oneShotRepeatableRepeatCount; ///< repeat count of one shot key
    bool                        m_isPrefix;                     ///< is prefix ?
    bool                        m_doesIgnoreModifierForPrefix;  ///< does ignore modifier key when prefixed ?
    bool                        m_doesEditNextModifier;         ///< does edit next user input key's modifier ?
    Modifier                    m_modifierForNextKey;           ///< modifier for next key if above is true

    /** current keymaps.
        <dl>
        <dt>when &amp;OtherWindowClass
        <dd>currentKeymap becoms currentKeymaps[++ Current::i]
        <dt>when &amp;KeymapParent
        <dd>currentKeymap becoms currentKeyamp-&gt;parentKeymap
        <dt>other
        <dd>currentKeyamp becoms *Current::i
        </dl>
      */
    const Keymap *volatile      m_currentKeymap;                ///< current keymap
    FocusOfThreads /*volatile*/ m_focusOfThreads;               ///<
    FocusOfThread *volatile     m_currentFocusOfThread;         ///<
    FocusOfThread               m_globalFocus;                  ///<
 #if 0 //defined(WIN32)
    HWND                        m_hwndFocus;                    ///< current focus window
 #endif
    DetachedThreadIds           m_detachedThreadIds;            ///<
    // for functions
    KeymapPtrList               m_keymapPrefixHistory;          ///< for &amp;KeymapPrevPrefix
 #if 0 //defined(WIN32)
    EmacsEditKillLine           m_emacsEditKillLine;            ///< for &amp;EmacsEditKillLine
 #endif
    const ActionFunction *      m_afShellExecute;               ///< for &amp;ShellExecute
 #if 0 //defined(WIN32)
    WindowPositions             m_windowPositions;              ///<
    WindowsWithAlpha            m_windowsWithAlpha;             ///<
 #endif
    tstring                     m_helpMessage;                  ///< for &amp;HelpMessage
    tstring                     m_helpTitle;                    ///< for &amp;HelpMessage
    int                         m_variable;                     ///< for &amp;Variable,
    ///  &amp;Repeat
public:
    tomsgstream&                m_log;                          ///< log stream (output to log dialog's edit)

private:
    /// keyboard handler thread
    static void keyboardHandler(void *i_this);
    ///
    void        keyboardHandler();
    /// check focus window
    void        checkFocusWindow();
    /// is modifier pressed ?
    bool        isPressed(Modifier::Type i_mt);
    /// fix modifier key
    bool        fixModifierKey(ModifiedKey *io_mkey, Keymap::AssignMode *o_am);
    /// output to log
    void        outputToLog(const Key *i_key, const ModifiedKey &i_mkey, int i_debugLevel);
    /// genete modifier events
    void        generateModifierEvents(const Modifier &i_mod);
    /// genete event
    void        generateEvents(Current i_c, const Keymap *i_keymap, Key *i_event);
    /// generate keyboard event
    void        generateKeyEvent(Key *i_key, bool i_doPress, bool i_isByAssign);
    ///
    void        generateActionEvents(const Current &i_c, const Action *i_a, bool i_doPress);
    ///
    void        generateKeySeqEvents(const Current &i_c, const KeySeq *i_keySeq, Part i_part);
    ///
    void        generateKeyboardEvents(const Current &i_c);
    ///
    void        beginGeneratingKeyboardEvents(const Current &i_c, bool i_isModifier);
    /// pop all pressed key on win32
    void        keyboardResetOnWin32();
    /// get current modifiers
    Modifier    getCurrentModifiers(Key *i_key, bool i_isPressed);
    /// describe bindings
    void        describeBindings();
    /// update m_lastPressedKey
    void        updateLastPressedKey(Key *i_key);
    /// set current keymap
    void        setCurrentKeymap(const Keymap * i_keymap, bool i_doesAddToHistory = false);
    /** open mayu device
        @return true if mayu device successfully is opened
      */
    bool        open();
    /// close mayu device
    void        close();

private:
    // BEGINING OF FUNCTION DEFINITION
    /// send a default key to Windows
    void        funcDefault(FunctionParam* i_param);
    /// use a corresponding key of a parent keymap
    void        funcKeymapParent(FunctionParam* i_param);
    /// use a corresponding key of a current window
    void        funcKeymapWindow(FunctionParam* i_param);
    /// use a corresponding key of the previous prefixed keymap
    void        funcKeymapPrevPrefix(FunctionParam* i_param, int i_previous);
    /// use a corresponding key of an other window class, or use a default key
    void        funcOtherWindowClass(FunctionParam* i_param);
    /// prefix key
    void        funcPrefix(FunctionParam* i_param, const Keymap *i_keymap,
                           BooleanType i_doesIgnoreModifiers = BooleanType_true);
    /// other keymap's key
    void        funcKeymap(FunctionParam* i_param, const Keymap *i_keymap);
    /// sync
    void        funcSync(FunctionParam* i_param);
    /// toggle lock
    void        funcToggle(FunctionParam* i_param, ModifierLockType i_lock, ToggleType i_toggle = ToggleType_toggle);
    /// edit next user input key's modifier
    void        funcEditNextModifier(FunctionParam* i_param, const Modifier& i_modifier);
    /// variable
    void        funcVariable(FunctionParam* i_param, int i_mag, int i_inc);
    /// repeat N times
    void        funcRepeat(FunctionParam* i_param, const KeySeq *i_keySeq, int i_max = 10);
    /// undefined (bell)
    void        funcUndefined(FunctionParam* i_param);
    /// ignore
    void        funcIgnore(FunctionParam* i_param);
 #if 0 //defined(WIN32)
    /// post message
    void        funcPostMessage(FunctionParam* i_param, ToWindowType i_window,
                                UINT i_message, WPARAM i_wParam, LPARAM i_lParam);
    /// ShellExecute
    void        funcShellExecute(FunctionParam* i_param, const StrExprArg &i_operation,
                                 const StrExprArg &i_file, const StrExprArg &i_parameters,
                                 const StrExprArg &i_directory,
                                 ShowCommandType i_showCommand);
    /// SetForegroundWindow
    void        funcSetForegroundWindow( FunctionParam* i_param,
                                        const tregex &i_windowClassName,
                                        LogicalOperatorType i_logicalOp = LogicalOperatorType_and,
                                        const tregex &i_windowTitleName = tregex( _T(".*") ) );
 #else // TODO:
 #endif
    /// load setting
    void        funcLoadSetting( FunctionParam* i_param, const StrExprArg &i_name = StrExprArg() );
    /// virtual key
    void        funcVK(FunctionParam* i_param, VKey i_vkey);
    /// wait
    void        funcWait(FunctionParam* i_param, int i_milliSecond);
    /// investigate WM_COMMAND, WM_SYSCOMMAND
    void        funcInvestigateCommand(FunctionParam* i_param);
    /// show mayu dialog box
    void        funcMayuDialog(FunctionParam* i_param, MayuDialogType i_dialog, ShowCommandType i_showCommand);
    /// describe bindings
    void        funcDescribeBindings(FunctionParam* i_param);
    /// show help message
    void        funcHelpMessage( FunctionParam* i_param,
                                const StrExprArg &i_title   = StrExprArg(),
                                const StrExprArg &i_message = StrExprArg() );
    /// show variable
    void        funcHelpVariable(FunctionParam* i_param, const StrExprArg &i_title);
    /// raise window
    void        funcWindowRaise(FunctionParam*  i_param, TargetWindowType   i_twt = TargetWindowType_overlapped);
    /// lower window
    void        funcWindowLower(FunctionParam*  i_param, TargetWindowType   i_twt = TargetWindowType_overlapped);
    /// minimize window
    void        funcWindowMinimize(FunctionParam* i_param, TargetWindowType i_twt = TargetWindowType_overlapped);
    /// maximize window
    void        funcWindowMaximize( FunctionParam* i_param, TargetWindowType i_twt = TargetWindowType_overlapped);
    /// maximize window horizontally
    void        funcWindowHMaximize(FunctionParam* i_param, TargetWindowType i_twt = TargetWindowType_overlapped);
    /// maximize window virtically
    void        funcWindowVMaximize(FunctionParam* i_param, TargetWindowType i_twt = TargetWindowType_overlapped);
    /// maximize window virtically or horizontally
    void        funcWindowHVMaximize(FunctionParam* i_param, BooleanType i_isHorizontal,
                                     TargetWindowType i_twt = TargetWindowType_overlapped);
    /// move window
    void        funcWindowMove(FunctionParam* i_param, int i_dx, int i_dy,
                                    TargetWindowType i_twt = TargetWindowType_overlapped);
    /// move window to ...
    void        funcWindowMoveTo(FunctionParam* i_param, GravityType i_gravityType,
                                 int i_dx, int i_dy, TargetWindowType i_twt = TargetWindowType_overlapped);
    /// move window visibly
    void        funcWindowMoveVisibly(FunctionParam* i_param, TargetWindowType  i_twt = TargetWindowType_overlapped);
    /// move window to other monitor
    void        funcWindowMonitorTo(FunctionParam* i_param,
                                    WindowMonitorFromType i_fromType, int i_monitor,
                                    BooleanType i_adjustPos  = BooleanType_true,
                                    BooleanType i_adjustSize = BooleanType_false);
    /// move window to other monitor
    void        funcWindowMonitor(FunctionParam* i_param, int i_monitor,
                                  BooleanType i_adjustPos  = BooleanType_true,
                                  BooleanType i_adjustSize = BooleanType_false);
    ///
    void        funcWindowClingToLeft(FunctionParam* i_param, TargetWindowType  i_twt = TargetWindowType_overlapped);
    ///
    void        funcWindowClingToRight(FunctionParam* i_param, TargetWindowType i_twt = TargetWindowType_overlapped);
    ///
    void        funcWindowClingToTop(FunctionParam* i_param, TargetWindowType   i_twt =  TargetWindowType_overlapped);
    ///
    void        funcWindowClingToBottom(FunctionParam*  i_param, TargetWindowType i_twt = TargetWindowType_overlapped);
    /// close window
    void        funcWindowClose(FunctionParam* i_param, TargetWindowType i_twt = TargetWindowType_overlapped);
    /// toggle top-most flag of the window
    void        funcWindowToggleTopMost(FunctionParam* i_param);
    /// identify the window
    void        funcWindowIdentify(FunctionParam* i_param);
    /// set alpha blending parameter to the window
    void        funcWindowSetAlpha(FunctionParam* i_param, int i_alpha);
    /// redraw the window
    void        funcWindowRedraw(FunctionParam* i_param);
    /// resize window to
    void        funcWindowResizeTo(FunctionParam* i_param, int i_width, int i_height,
                                   TargetWindowType i_twt = TargetWindowType_overlapped);
    /// move the mouse cursor
    void        funcMouseMove(FunctionParam* i_param, int i_dx, int i_dy);
    /// send a mouse-wheel-message to Windows
    void        funcMouseWheel(FunctionParam* i_param, int i_delta);
    /// convert the contents of the Clipboard to upper case or lower case
    void        funcClipboardChangeCase(FunctionParam * i_param, BooleanType i_doesConvertToUpperCase);
    /// convert the contents of the Clipboard to upper case
    void        funcClipboardUpcaseWord(FunctionParam* i_param);
    /// convert the contents of the Clipboard to lower case
    void        funcClipboardDowncaseWord(FunctionParam* i_param);
    /// set the contents of the Clipboard to the string
    void        funcClipboardCopy(FunctionParam* i_param, const StrExprArg &i_text);
    ///
    void        funcEmacsEditKillLinePred(FunctionParam * i_param, const KeySeq* i_keySeq1, const KeySeq* i_keySeq2);
    ///
    void        funcEmacsEditKillLineFunc(FunctionParam* i_param);
    /// clear log
    void        funcLogClear(FunctionParam* i_param);
    /// recenter
    void        funcRecenter(FunctionParam* i_param);
    /// Direct SSTP
    void        funcDirectSSTP(FunctionParam *              i_param,
                               const tregex &               i_name,
                               const StrExprArg &           i_protocol,
                               const std::list<tstringq> &  i_headers);
 #if 0 //defined(WIN32)
    /// PlugIn
    void        funcPlugIn(FunctionParam *      i_param,
                           const StrExprArg &   i_dllName,
                           const StrExprArg &   i_funcName = StrExprArg(),
                           const StrExprArg &   i_funcParam = StrExprArg(),
                           BooleanType          i_doesCreateThread = BooleanType_false);
 #endif
    /// set IME open status
    void        funcSetImeStatus(FunctionParam* i_param, ToggleType i_toggle = ToggleType_toggle);
    /// set string to IME
    void        funcSetImeString(FunctionParam* i_param, const StrExprArg &i_data);

    // END OF FUNCTION DEFINITION

 #define FUNCTION_FRIEND
 #include "functions.h"
 #undef FUNCTION_FRIEND

public:
    ///
    Engine(tomsgstream &i_log);
    ///
    ~Engine();
    /// start/stop keyboard handler thread
    void    start();
    ///
    void    stop();
    /// pause keyboard handler thread and close device
    bool    pause();
    /// resume keyboard handler thread and re-open device
    bool    resume();
    /// logging mode
    void enableLogMode(bool i_isLogMode = true) { m_isLogMode = i_isLogMode; }
    ///
    void disableLogMode() { m_isLogMode = false; }
    /// enable/disable engine
    void enable(bool i_isEnabled = true) { m_isEnabled = i_isEnabled; }
    ///
    void disable() { m_isEnabled = false; }
    ///
    bool getIsEnabled() const { return m_isEnabled; }
 #if 0 //defined(WIN32)
    /// associated window
    void setAssociatedWndow(HWND i_hwnd) { m_hwndAssocWindow = i_hwnd; }
    /// associated window
    HWND getAssociatedWndow() const { return m_hwndAssocWindow; }
 #endif
    /// setting
    bool    setSetting(Setting *i_setting);
 #if 0 //defined(WIN32)
    /// focus
    bool    setFocus(HWND i_hwndFocus, DWORD i_threadId,
                     const tstringi &i_className,
                     const tstringi &i_titleName, bool i_isConsole);
 #endif
    /// lock state
    bool    setLockState(bool i_isNumLockToggled, bool i_isCapsLockToggled,
                         bool i_isScrollLockToggled, bool i_isKanaLockToggled,
                         bool i_isImeLockToggled, bool i_isImeCompToggled);
 #if 0 //defined(WIN32)
    /// show
    void    checkShow(HWND i_hwnd);
    bool    setShow(bool i_isMaximized, bool i_isMinimized, bool i_isMDI);
 #endif
 #if 0 //defined(WIN32)
    /// sync
    bool    syncNotify();
    /// thread detach notify
    bool    threadDetachNotify(uint32_t i_threadId);
 #endif
    /// shell execute
    void    shellExecute();
    /// get help message
    void    getHelpMessages(tstring *o_helpMessage, tstring *o_helpTitle);
 #if 0 //defined(WIN32)
    /// command notify
    void    commandNotify(HWND i_hwnd, UINT i_message, WPARAM i_wParam, LPARAM i_lParam);
 #endif
    /// get current window class name
    const tstringi &getCurrentWindowClassName() const { return m_currentFocusOfThread->m_className; }
    /// get current window title name
    const tstringi &getCurrentWindowTitleName() const { return m_currentFocusOfThread->m_titleName; }
    /// get mayud version
    const tstring & getMayudVersion() const { return m_mayudVersion; }
};


///
class FunctionParam
{
public:
    bool                    m_isPressed;    ///< is key pressed ?
 #if 0 //defined(WIN32)
    HWND                    m_hwnd;         ///<
 #endif
    Engine::Current         m_c;            ///< new context
    bool                    m_doesNeedEndl; ///< need endl ?
    const ActionFunction *  m_af;           ///<
};


#endif // !_ENGINE_H
