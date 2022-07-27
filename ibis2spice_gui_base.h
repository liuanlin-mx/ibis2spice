///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/filepicker.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ibis2spice_gui_base
///////////////////////////////////////////////////////////////////////////////
class ibis2spice_gui_base : public wxFrame
{
	private:

	protected:
		wxListBox* m_listBoxComponent;
		wxFilePickerCtrl* m_filePicker;
		wxListBox* m_listBoxModel;
		wxTextCtrl* m_textCtrlFilter;
		wxListBox* m_listBoxModelSelected;
		wxCheckBox* m_checkBoxTyp;
		wxCheckBox* m_checkBoxMin;
		wxCheckBox* m_checkBoxMax;
		wxButton* m_buttonAdd;
		wxButton* m_buttonDel;
		wxButton* m_buttonSave;
		wxButton* m_buttonConvert;

		// Virtual event handlers, override them in your derived class
		virtual void m_filePickerOnFileChanged( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void m_textCtrlFilterOnText( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonAddOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonDelOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonSaveOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonConvertOnButtonClick( wxCommandEvent& event ) { event.Skip(); }


	public:

		ibis2spice_gui_base( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("ibis2spice"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 800,600 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL, const wxString& name = wxT("ibis2spice") );

		~ibis2spice_gui_base();

};

