#include <iostream>
#include <wx/wx.h>
#include "ConvertFunctions.hpp"

class MainFrame : public wxFrame {
public:
	MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title) {
		constexpr unsigned int COLS = 2;
		constexpr unsigned int ROWS = 2;

		const auto margin = FromDIP(30);

		auto mainSizer = new wxBoxSizer(wxVERTICAL);
		auto panel = new wxPanel(
				this,
			   	wxID_ANY
		);

		panel->SetBackgroundColour(this->GetBackgroundColour());
		
		auto sizer = new wxFlexGridSizer(ROWS, COLS, margin, margin);
		sizer->AddGrowableCol(0);
		sizer->AddGrowableCol(1);
		sizer->AddGrowableRow(1);

		auto timesName = new wxStaticText(panel, wxID_ANY, "ANSI text");
		sizer->Add(timesName, 0, wxALIGN_CENTER_HORIZONTAL);

		auto unicodeName = new wxStaticText(panel, wxID_ANY, "Unicode text");
		sizer->Add(unicodeName, 0, wxALIGN_CENTER_HORIZONTAL);

		auto timesInput = new wxTextCtrl(panel,
			   	wxID_ANY,
			   	wxEmptyString,
			   	wxDefaultPosition,
			   	wxSize(250, -1),
			   	wxTE_MULTILINE
		);
		sizer->Add(timesInput, 1, wxEXPAND);
		
		auto unicodeInput = new wxTextCtrl(panel,
			   	wxID_ANY,
			   	wxEmptyString,
			   	wxDefaultPosition,
			   	wxSize(250, -1),
			   	wxTE_MULTILINE
		);
		sizer->Add(unicodeInput, 1, wxEXPAND);
		
		sizer->SetMinSize(FromDIP(640), wxDefaultSize.GetHeight());
		panel->SetSizer(sizer);

		// Mutex to avoid chain reaction
		bool converting = false;

		timesInput->Bind(wxEVT_TEXT, 
			[this, unicodeInput, &converting](wxCommandEvent& e) {
					if(!converting) {
						converting = true;
						std::string raw_text = e.GetString().utf8_string();
						// std::cout << "Raw: " << raw_text << std::endl;
						std::string res = TimesToUnicode(raw_text);
						// std::cout << res << std::endl;
						unicodeInput->ChangeValue(wxString::FromUTF8(res));
						converting = false;
					}
				});

		unicodeInput->Bind(wxEVT_TEXT, 
			[this, timesInput, &converting](wxCommandEvent& e) {
					if(!converting) {
						converting = true;
						std::string raw_text = e.GetString().utf8_string();
						// std::cout << "Raw: " << raw_text << std::endl;
						std::string res = UnicodeToTimes(raw_text);
						// std::cout << res << std::endl;
						timesInput->ChangeValue(wxString::FromUTF8(res));
						converting = false;
					}
				});

		mainSizer->Add(panel, 1, wxEXPAND | wxALL, margin);
		this->SetSizerAndFit(mainSizer);
	}
};


class App : public wxApp {
public:
	bool OnInit() {
		MainFrame* mf = new MainFrame("Converter");
		mf->Center();
		mf->Show();
		return true;
	}
};

wxIMPLEMENT_APP(App);

