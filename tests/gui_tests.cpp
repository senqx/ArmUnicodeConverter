#include <wx/wx.h>
#include <wx/evtloop.h>
#undef wxIMPLEMENT_APP
#define wxIMPLEMENT_APP(app) wxIMPLEMENT_APP_NO_MAIN(app)
#include "../Main.cpp"
#include <chrono>
#include <iostream>
#include <stdexcept>

void expect(bool condition, const char* message) {
  if (!condition) throw std::runtime_error(message);
}
void pumpFor(int milliseconds) {
  const auto deadline=std::chrono::steady_clock::now()+std::chrono::milliseconds(milliseconds);
  while(std::chrono::steady_clock::now()<deadline){wxYield();wxMilliSleep(1);}
}
void waitFor(wxStyledTextCtrl* control, const wxString& expected) {
  const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(30);
  while(control->GetText() != expected) {
    wxYield(); wxMilliSleep(1);
    if(std::chrono::steady_clock::now() > deadline) throw std::runtime_error("GUI update timed out");
  }
}
int main(int argc, char** argv) {
  if (!wxEntryStart(argc,argv)) return 1;
  wxTheApp->CallOnInit();
  int result=0;
  try {
    wxGUIEventLoop eventLoop;
    wxEventLoopActivator activate(&eventLoop);
    auto* legacy = dynamic_cast<wxStyledTextCtrl*>(wxWindow::FindWindowByName("legacy-input"));
    auto* unicode = dynamic_cast<wxStyledTextCtrl*>(wxWindow::FindWindowByName("unicode-input"));
    expect(legacy && unicode,"Missing editor controls");
    unicode->SetText(wxString::FromUTF8(u8"ԱԲԳ"));
    waitFor(legacy,wxString::FromUTF8(u8"²´¶"));
    std::cout << "PASS real GUI conversion and debounce\n";
    std::string medium(65536,'a');
    unicode->SetText(wxString::FromUTF8(medium));
    for(int edit=0;edit<3;++edit) {
      pumpFor(250);
      expect(legacy->GetText()==wxString::FromUTF8(u8"²´¶"),"Large text updated during typing");
      unicode->AppendText("x");
    }
    waitFor(legacy,wxString::FromUTF8(medium + "xxx"));
    std::cout << "PASS large text waits for 750 ms after the last edit\n";
    std::string large;
    while(large.size()<10000000) large+=u8"ԱԲԳ հայերեն 你好 🙂\n";
    auto start=std::chrono::steady_clock::now();
    unicode->SetText(wxString::FromUTF8(large));
    waitFor(legacy,wxString::FromUTF8(UnicodeToTimes(large)));
    std::cout << "PASS 10 MB paste through real widgets: "
      << std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count()<<" s\n";
    // Start a large conversion, then change direction while it is pending.
    unicode->SetText(wxString::FromUTF8(large + "x"));
    pumpFor(160);
    legacy->SetText(wxString::FromUTF8(u8"²´¶"));
    waitFor(unicode,wxString::FromUTF8(u8"ԱԲԳ"));
    pumpFor(300);
    expect(unicode->GetText()==wxString::FromUTF8(u8"ԱԲԳ"),"Stale result replaced latest edit");
    std::cout << "PASS editing opposite pane cancels stale large conversion\n";
    unicode->SetText(wxString::FromUTF8(u8"Ա"));
    unicode->SetText(wxString::FromUTF8(u8"ԱԲ"));
    unicode->SetText(wxString::FromUTF8(u8"ԱԲԳ"));
    waitFor(legacy,wxString::FromUTF8(u8"²´¶"));
    std::cout << "PASS rapid typing keeps latest result\n";
    unicode->SetText(wxString::FromUTF8(large));
    wxTheApp->GetTopWindow()->Destroy();
    wxYield();
    std::cout << "PASS closing with pending conversion\n";
  } catch(const std::exception& error){std::cerr<<error.what()<<'\n';result=1;}
  wxTheApp->OnExit();wxEntryCleanup();return result;
}
