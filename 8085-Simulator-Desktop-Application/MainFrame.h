#pragma once
#include<wx\wx.h>
#include<wx\artprov.h>
#include<wx/stc/stc.h>
#include<wx/listctrl.h>
#include"wx/html/htmlwin.h"
#include<map>
#include"constants.h"



class MainFrame : public wxFrame
{
private:

	//Menu Bar
	wxMenuBar* m_MenuBar = nullptr;
	wxMenu* m_FileMenu = nullptr;
	wxMenu* m_HelpMenu = nullptr;

	//Tool Bar
	wxToolBar* m_ToolBar = nullptr;


	//Text Editor
	wxPanel* m_TextBoxPanel = nullptr;
	wxStyledTextCtrl* m_EditBox = nullptr;
	wxStaticBox* m_TextBoxStaticBox = nullptr;
	wxStaticBoxSizer* m_TextBoxStaticBoxSizer = nullptr;

	//File
	wxString m_currentFilePath;

	//Program Loading Panel
	wxPanel* m_ProgramLoadingPanel = nullptr;
	wxStaticBox* m_ProgramLoadingPanelStaticBox = nullptr;
	wxStaticBoxSizer* m_ProgramLoadingPanelStaticBoxSizer = nullptr;
	wxTextCtrl* m_ProgramLoadingPanelTextCtrl = nullptr;
	wxTextCtrl* m_ProgramLoadingPanelCurrentLocationTextCtrl = nullptr;
	wxStaticText* m_ProgramLoadingPanelLabel = nullptr;
	wxStaticText* m_ProgramLoadingPanelCurrentLocationTextLabel = nullptr;
	wxButton* m_ProgramLoadingPanelSetButton = nullptr;


	//Flag Rgister Panel 
	wxPanel* m_FlagPanel = nullptr;
	wxStaticBox* m_FlagPanelStaticBox = nullptr;
	wxStaticBoxSizer* m_FlagPanelStaticBoxSizer = nullptr;
	wxCheckListBox* m_FlagRegCheckList = nullptr;



	//Main Register Panel
	wxPanel* m_RegisterPanel = nullptr;
	wxStaticBox* m_RegisterPanelStaticBox = nullptr;
	wxStaticBoxSizer* m_RegisterPanelStaticBoxSizer = nullptr;
	std::map<std::string, wxTextCtrl*> m_MainRegister;
	std::map<std::string, wxStaticText*> m_MainRegisterLabel;
	wxTextCtrl* m_PC_TextCtrl = nullptr;
	wxStaticText* m_PC_StaticText = nullptr;
	wxTextCtrl* m_SP_TextCtrl = nullptr;
	wxStaticText* m_SP_StaticText = nullptr;
	wxTextCtrl* m_PSW_TextCtrl = nullptr;
	wxStaticText* m_PSW_StaticText = nullptr;



	//Memroy Initializer Panel
	wxPanel* m_MemoryInitPanel = nullptr;
	wxStaticBox* m_MemoryInitPanelStaticBox = nullptr;
	wxStaticBoxSizer* m_MemoryInitPanelStaticBoxSizer = nullptr;
	wxButton* m_SetButton = nullptr;
	wxStaticText* m_MemoryLocationLabel = nullptr;
	wxTextCtrl* m_MemoryAddressTextCtrl = nullptr;
	wxStaticText* m_DataLabel = nullptr;
	wxTextCtrl* m_DataTextCtrl = nullptr;



	//Memroy Viewer Panel
	wxPanel* m_MemoryViewPanel = nullptr;
	wxStaticBox* m_MemoryViewPanelStaticBox = nullptr;
	wxStaticBoxSizer* m_MemoryViewPanelStaticBoxSizer = nullptr;
	wxButton* m_ViewButton = nullptr;
	wxStaticText* m_FromLabel = nullptr;
	wxTextCtrl* m_CountTextCtrl = nullptr;
	wxStaticText* m_CountLabel = nullptr;
	wxTextCtrl* m_FromMemoryAddressTextCtrl = nullptr;
	wxListView* m_MemoryViewList = nullptr;

	//Debug Panel
	wxPanel* m_DebugPanel = nullptr;
	wxStaticText* m_CurrentLineLabel = nullptr;
	wxTextCtrl* m_CurrentLineTextCtrl = nullptr;
	wxButton* m_ExecuteButton = nullptr;
	wxButton* m_DebugButton = nullptr;
	wxButton* m_StopButton = nullptr;
	wxStaticBox* m_DebugStaticBox = nullptr;
	wxStaticBoxSizer* m_DebugStaticBoxSizer = nullptr;


	//Sizers
	wxBoxSizer* m_MainSizer = nullptr;
	wxBoxSizer* m_LeftPanelSizer = nullptr;
	wxBoxSizer* m_RightPanelSizer = nullptr;

	//Other
	const std::string m_MainRegisterArray[7] = { REGISTER::A, REGISTER::B, REGISTER::C, REGISTER::D, REGISTER::E, REGISTER::H, REGISTER::L };
	const char* m_FlagRegisterArray[5] = { "S", "Z", "AC", "P", "CY" };

	//State
	bool m_bDebugMode = false;
	int m_nLoadingLocation = 0;//Current Program Loading Location
public:



	MainFrame();
	~MainFrame();

	void OnOpen(wxCommandEvent& event);
	void OnSave(wxCommandEvent& event);
	void OnSaveAs(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnRun(wxCommandEvent& envet);
	void OnSet(wxCommandEvent& envet);
	void OnSetLoadingLocation(wxCommandEvent& envet);
	void OnLoadProgram(wxCommandEvent& event);
	void LoadProgram(const std::string&);
	void OnView(wxCommandEvent& envet);
	void UpdateFlagRegister();
	void UpdateRegisters();
	void UpdateMemory();
	void OnDebug(wxCommandEvent& event);
	void OnStopDebug(wxCommandEvent& event);
	void OnExecute(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnHelp(wxCommandEvent& event);
	void Clear();//Will be called before running the program(Memory will not be affected)
	void Run8085(const std::string& filePath);
	void Debug8085(const std::string& filePath);
	void Init();
	DECLARE_EVENT_TABLE();
};

enum ButtonID
{
	VIEW_BUTTON = 333,
	SET_BUTTON,
	EXECUTE_BUTTON,
	DEBUG_BUTTON,
	STOP_BUTTON,
	SET_LOADING_LOCATION_BUTTON,
	ID_LOAD,
};


