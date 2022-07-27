///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ibis2spice_gui_base.h"

///////////////////////////////////////////////////////////////////////////

ibis2spice_gui_base::ibis2spice_gui_base( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxFrame( parent, id, title, pos, size, style, name )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );

	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("IBIS Component") ), wxVERTICAL );

	m_listBoxComponent = new wxListBox( sbSizer1->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	sbSizer1->Add( m_listBoxComponent, 1, wxALL|wxEXPAND, 5 );

	m_filePicker = new wxFilePickerCtrl( sbSizer1->GetStaticBox(), wxID_ANY, wxEmptyString, wxT("Select IBIS Model file"), wxT("*.ibs"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE );
	sbSizer1->Add( m_filePicker, 0, wxALL|wxEXPAND, 5 );


	bSizer1->Add( sbSizer1, 1, wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("IBIS Model") ), wxVERTICAL );

	m_listBoxModel = new wxListBox( sbSizer2->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_MULTIPLE );
	sbSizer2->Add( m_listBoxModel, 1, wxALL|wxEXPAND, 5 );

	m_textCtrlFilter = new wxTextCtrl( sbSizer2->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_textCtrlFilter, 0, wxALL|wxEXPAND, 5 );


	bSizer1->Add( sbSizer2, 1, wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("IBIS Model Selected") ), wxVERTICAL );

	m_listBoxModelSelected = new wxListBox( sbSizer3->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_MULTIPLE );
	sbSizer3->Add( m_listBoxModelSelected, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );

	m_checkBoxTyp = new wxCheckBox( sbSizer3->GetStaticBox(), wxID_ANY, wxT("Typ"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxTyp->SetValue(true);
	bSizer3->Add( m_checkBoxTyp, 1, wxALL, 5 );

	m_checkBoxMin = new wxCheckBox( sbSizer3->GetStaticBox(), wxID_ANY, wxT("Min"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_checkBoxMin, 1, wxALL, 5 );

	m_checkBoxMax = new wxCheckBox( sbSizer3->GetStaticBox(), wxID_ANY, wxT("Max"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_checkBoxMax, 0, wxALL, 5 );


	sbSizer3->Add( bSizer3, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );

	m_buttonAdd = new wxButton( sbSizer3->GetStaticBox(), wxID_ANY, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonAdd, 1, wxEXPAND, 5 );

	m_buttonDel = new wxButton( sbSizer3->GetStaticBox(), wxID_ANY, wxT("Del"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonDel, 1, wxEXPAND, 5 );


	sbSizer3->Add( bSizer2, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );

	m_buttonSave = new wxButton( sbSizer3->GetStaticBox(), wxID_ANY, wxT("Save"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_buttonSave, 1, wxEXPAND, 5 );

	m_buttonConvert = new wxButton( sbSizer3->GetStaticBox(), wxID_ANY, wxT("Convert"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_buttonConvert, 1, wxEXPAND, 5 );


	sbSizer3->Add( bSizer4, 0, wxEXPAND, 5 );


	bSizer1->Add( sbSizer3, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_filePicker->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( ibis2spice_gui_base::m_filePickerOnFileChanged ), NULL, this );
	m_textCtrlFilter->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ibis2spice_gui_base::m_textCtrlFilterOnText ), NULL, this );
	m_buttonAdd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ibis2spice_gui_base::m_buttonAddOnButtonClick ), NULL, this );
	m_buttonDel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ibis2spice_gui_base::m_buttonDelOnButtonClick ), NULL, this );
	m_buttonSave->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ibis2spice_gui_base::m_buttonSaveOnButtonClick ), NULL, this );
	m_buttonConvert->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ibis2spice_gui_base::m_buttonConvertOnButtonClick ), NULL, this );
}

ibis2spice_gui_base::~ibis2spice_gui_base()
{
	// Disconnect Events
	m_filePicker->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( ibis2spice_gui_base::m_filePickerOnFileChanged ), NULL, this );
	m_textCtrlFilter->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ibis2spice_gui_base::m_textCtrlFilterOnText ), NULL, this );
	m_buttonAdd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ibis2spice_gui_base::m_buttonAddOnButtonClick ), NULL, this );
	m_buttonDel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ibis2spice_gui_base::m_buttonDelOnButtonClick ), NULL, this );
	m_buttonSave->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ibis2spice_gui_base::m_buttonSaveOnButtonClick ), NULL, this );
	m_buttonConvert->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ibis2spice_gui_base::m_buttonConvertOnButtonClick ), NULL, this );

}
