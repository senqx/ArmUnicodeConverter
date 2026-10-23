#include <wx/wx.h>
#include <wx/stc/stc.h>
#include <wx/clipbrd.h>
#include <wx/dataobj.h>
#include "LiveConversion.hpp"

class MainFrame : public wxFrame {
  wxStyledTextCtrl* legacy;
  wxStyledTextCtrl* unicode;
  wxTimer timer;
  LiveConversion conversion;
  bool toUnicode = true;
  bool started = false;
  bool applying = false;

  void schedule(bool direction) {
    toUnicode = direction;
    started = false;
    // GetTextLength reads the UTF-8 byte count without copying the document.
    // One conversion chunk stays live; larger documents wait for a typing pause.
    const bool live = (direction ? legacy : unicode)->GetTextLength() <= 32768;
    timer.StartOnce(live ? 1 : 750);
    SetStatusText(live ? "Converting…" : "Waiting for typing to finish…");
  }
  void advance(wxTimerEvent&) {
    if (!started) {
      conversion.start((toUnicode ? legacy : unicode)->GetText().utf8_string(), toUnicode);
      started = true;
      SetStatusText("Converting…");
    }
    if (!conversion.step()) { timer.StartOnce(1); return; }
    // Suppress programmatic modifications and replace once to avoid repeated layout.
    applying = true;
    auto* target = toUnicode ? unicode : legacy;
    target->SetText(wxString::FromUTF8(conversion.result()));
    applying = false;
    SetStatusText("Up to date");
  }
public:
  MainFrame() : wxFrame(nullptr, wxID_ANY, "Armenian Unicode Converter"), timer(this) {
    const wxColour background(242,244,247), ink(35,47,62), muted(91,105,121);
    auto* panel = new wxPanel(this);
    panel->SetBackgroundColour(background);
    auto* inner = new wxBoxSizer(wxVERTICAL);
    auto* heading = new wxStaticText(panel, wxID_ANY, "Armenian text converter");
    auto titleFont = GetFont(); titleFont.SetPointSize(20); titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    heading->SetFont(titleFont); heading->SetForegroundColour(ink);
    inner->Add(heading,0,wxLEFT|wxRIGHT|wxTOP,FromDIP(24));
    auto* description = new wxStaticText(panel,wxID_ANY,"Paste or type in either pane. The other updates automatically.");
    description->SetForegroundColour(muted);
    inner->Add(description,0,wxLEFT|wxRIGHT|wxTOP|wxBOTTOM,FromDIP(24));
    auto* columns = new wxBoxSizer(wxHORIZONTAL);
    auto makePane = [&](const wxString& title,const wxString& subtitle,wxStyledTextCtrl*& editor) {
      auto* card = new wxPanel(panel);
      card->SetBackgroundColour(*wxWHITE);
      auto* layout = new wxBoxSizer(wxVERTICAL);
      auto* label = new wxStaticText(card,wxID_ANY,title);
      auto labelFont = GetFont(); labelFont.SetPointSize(12);labelFont.SetWeight(wxFONTWEIGHT_BOLD);
      label->SetFont(labelFont);label->SetForegroundColour(ink);
      layout->Add(label,0,wxLEFT|wxRIGHT|wxTOP,FromDIP(16));
      auto* hint = new wxStaticText(card,wxID_ANY,subtitle);
      hint->SetForegroundColour(muted);
      layout->Add(hint,0,wxLEFT|wxRIGHT|wxTOP|wxBOTTOM,FromDIP(16));
      editor = new wxStyledTextCtrl(card,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxBORDER_NONE);
      editor->SetCodePage(wxSTC_CP_UTF8);
      editor->SetWrapMode(wxSTC_WRAP_NONE);
      for(int margin=0;margin<5;++margin) editor->SetMarginWidth(margin,0);
      auto textFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT); textFont.SetPointSize(12);
      editor->StyleSetFont(wxSTC_STYLE_DEFAULT,textFont);
      editor->StyleSetForeground(wxSTC_STYLE_DEFAULT,ink);
      editor->StyleSetBackground(wxSTC_STYLE_DEFAULT,*wxWHITE);
      editor->StyleClearAll();
      editor->SetCaretForeground(wxColour(49,105,155));
      editor->SetSelBackground(true,wxColour(216,233,248));
      editor->SetSelForeground(true,ink);
      editor->SetTabWidth(4);
      layout->Add(editor,1,wxEXPAND|wxLEFT|wxRIGHT,FromDIP(16));
      auto* copy = new wxButton(card,wxID_ANY,"Copy text");
      copy->Bind(wxEVT_BUTTON,[editor](wxCommandEvent&) {
        if(wxTheClipboard->Open()) {
          wxTheClipboard->SetData(new wxTextDataObject(editor->GetText()));
          wxTheClipboard->Close();
        }
      });
      layout->Add(copy,0,wxALIGN_RIGHT|wxALL,FromDIP(12));
      card->SetSizer(layout);
      return card;
    };
    columns->Add(makePane("Legacy Times Armenian","Text copied from a legacy document",legacy),1,wxEXPAND|wxRIGHT,FromDIP(16));
    columns->Add(makePane("Armenian Unicode","Text for modern apps and documents",unicode),1,wxEXPAND);
    legacy->SetName("legacy-input");unicode->SetName("unicode-input");
    inner->Add(columns,1,wxEXPAND|wxLEFT|wxRIGHT,FromDIP(24));
    auto* note = new wxStaticText(panel,wxID_ANY,"Large texts update after a short typing pause. Your text stays on this device.");
    note->SetForegroundColour(muted);
    inner->Add(note,0,wxALL,FromDIP(24));
    panel->SetSizer(inner);
    auto* outer = new wxBoxSizer(wxVERTICAL);outer->Add(panel,1,wxEXPAND);SetSizer(outer);
    SetClientSize(FromDIP(wxSize(1000,640)));
    SetMinClientSize(FromDIP(wxSize(760,440)));
    CreateStatusBar();SetStatusText("Type or paste text to begin");
    legacy->Bind(wxEVT_STC_MODIFIED, [this](wxStyledTextEvent& event) {
      if(!applying && (event.GetModificationType() & (wxSTC_MOD_INSERTTEXT | wxSTC_MOD_DELETETEXT))) schedule(true);
    });
    unicode->Bind(wxEVT_STC_MODIFIED, [this](wxStyledTextEvent& event) {
      if(!applying && (event.GetModificationType() & (wxSTC_MOD_INSERTTEXT | wxSTC_MOD_DELETETEXT))) schedule(false);
    });
    Bind(wxEVT_TIMER, &MainFrame::advance, this);
  }
};
class App : public wxApp {
public:
  bool OnInit() override {
    auto* frame = new MainFrame(); frame->Centre(); frame->Show(); return true;
  }
};
wxIMPLEMENT_APP(App);
