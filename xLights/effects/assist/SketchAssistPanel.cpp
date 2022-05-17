#include "SketchAssistPanel.h"
#include "SketchCanvasPanel.h"

#include "../../xSchedule/wxJSON/jsonreader.h"
#include "../../xSchedule/wxJSON/jsonwriter.h"
#include "UtilFunctions.h"
#include <xLightsMain.h>

#include <wx/button.h>
#include <wx/listbox.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/filepicker.h>
#include <wx/wfstream.h>

namespace
{
    const wxString imgSelect("Select an image file");
    const wxString imgFilters("*.jpg;*.gif;*.png;*.bmp;*.jpeg");

    const char HotkeysText[] =
        "Shift\tToggle segment type (line, one-point curve, two-point curve)\n"
        "Esc\tEnd current path\n"
        "Space\tClose current path\n"
        "Delete\tDelete point/segment\n\n"
        "Use mouse wheel and middle button to zoom and pan on sketch";
}

BEGIN_EVENT_TABLE(SketchAssistPanel, wxPanel)
END_EVENT_TABLE()

long SketchAssistPanel::ID_MENU_Delete = wxNewId();
long SketchAssistPanel::ID_MENU_Reverse = wxNewId();

SketchAssistPanel::SketchAssistPanel(wxWindow* parent, wxWindowID id /*wxID_ANY*/, const wxPoint& pos /*wxDefaultPosition*/, const wxSize& size /*wxDefaultSize*/) :
    wxPanel(parent, id, pos, size)
{
    /*
 
    mainSizer
     \
      pathUISizer (canvas, bg-image controls, and path/sketch controls)
      |\
      | canvasFrame (m_sketchCanvasPanel)
      |\
      | pathSketchCtrlsSizer
      | |\
      | | pathCtrlsSizer (m_startPathBtn, m_endPathBtn, m_closePathBtn)
      | |
      |  \
      |   sketchCtrlsSizer ("Clear" sketch button)
      |
      sketchUISizer (hotkeys text and paths listbox)
      |\
      | hotkeysSizer (Canvas hotkeys)
      |
      |\
      | pathsSizer (m_pathsListBox)

    */
    auto mainSizer = new wxFlexGridSizer(3, 1, 0, 0);
    mainSizer->AddGrowableCol(0);
    mainSizer->AddGrowableRow(0);

    auto pathUISizer = new wxFlexGridSizer(/*3*/2, 1, 5, 0);
    pathUISizer->AddGrowableRow(0);
    pathUISizer->AddGrowableCol(0);

    auto pathSketchCtrlsSizer = new wxFlexGridSizer(1, 3, 0, 5);
    auto sketchCtrlsSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Sketch");

    auto importCtrlsSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Import/Export");

    auto sketchUISizer = new wxFlexGridSizer(2, 1, 5, 0);
    sketchUISizer->AddGrowableRow(1);
    sketchUISizer->AddGrowableCol(0);

    auto hotkeysSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Canvas hotkeys");
    auto pathsSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Paths");

    // canvas
    auto canvasFrame = new wxStaticBox(this, wxID_ANY, wxEmptyString);
    auto canvasFrameSizer = new wxFlexGridSizer(1, 1, 0, 0);
    canvasFrameSizer->AddGrowableRow(0);
    canvasFrameSizer->AddGrowableCol(0);
    m_sketchCanvasPanel = new SketchCanvasPanel(this, canvasFrame, wxID_ANY, wxDefaultPosition, wxSize(400, 300));
    canvasFrameSizer->Add(m_sketchCanvasPanel, 1, wxALL | wxEXPAND);
    canvasFrame->SetSizer(canvasFrameSizer);

    // path / sketch controls
    m_startPathBtn = new wxButton(this, wxID_ANY, "Start");
    m_endPathBtn = new wxButton(this, wxID_ANY, "End");
    m_closePathBtn = new wxButton(this, wxID_ANY, "Close");
    m_continuePathBtn = new wxButton(this, wxID_ANY, "Continue");

    m_clearSketchBtn = new wxButton(this, wxID_ANY, "Clear");
    m_importSketchBtn = new wxButton(this, wxID_ANY, "Import");
    m_exportSketchBtn = new wxButton(this, wxID_ANY, "Export");

    auto pathCtrlsSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Path");
    pathCtrlsSizer->Add(m_startPathBtn, 1, wxALL | wxEXPAND, 3);
    pathCtrlsSizer->Add(m_endPathBtn, 1, wxALL | wxEXPAND, 3);
    pathCtrlsSizer->Add(m_closePathBtn, 1, wxALL | wxEXPAND, 3);
    pathCtrlsSizer->Add(m_continuePathBtn, 1, wxALL | wxEXPAND, 3);

    sketchCtrlsSizer->Add(m_clearSketchBtn, 1, wxALL | wxEXPAND, 3);

    importCtrlsSizer->Add(m_importSketchBtn, 1, wxALL | wxEXPAND, 3);
    importCtrlsSizer->Add(m_exportSketchBtn, 1, wxALL | wxEXPAND, 3);

    pathSketchCtrlsSizer->AddGrowableCol(2);
    pathSketchCtrlsSizer->Add(pathCtrlsSizer, 1, wxALL, 2);
    pathSketchCtrlsSizer->Add(sketchCtrlsSizer, 1, wxALL, 2);
    pathSketchCtrlsSizer->Add(importCtrlsSizer, 1, wxALL, 2);

    // Hotkeys text
    hotkeysSizer->Add(new wxStaticText(hotkeysSizer->GetStaticBox(), wxID_ANY, HotkeysText), 1, wxALL | wxEXPAND, 3);

    // Paths ListBox
    m_pathsListBox = new wxListBox(this, wxID_ANY);
    pathsSizer->Add(m_pathsListBox, 1, wxALL | wxEXPAND, 3);

    sketchUISizer->Add(hotkeysSizer, 1, wxALL | wxEXPAND);
    sketchUISizer->Add(pathsSizer, 1, wxALL | wxEXPAND, 5);

    pathUISizer->Add(canvasFrame, 1, wxALL | wxEXPAND, 5);
    pathUISizer->Add(pathSketchCtrlsSizer, 1, wxALL | wxEXPAND, 5);

    mainSizer->Add(pathUISizer, 1, wxALL | wxEXPAND, 5);
    mainSizer->Add(sketchUISizer, 1, wxALL | wxEXPAND, 5);

    SetSizer(mainSizer);

    SetName("ID_PANEL_SKETCH_ASSIST");

    Connect(m_startPathBtn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&SketchAssistPanel::OnButton_StartPath);
    Connect(m_endPathBtn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&SketchAssistPanel::OnButton_EndPath);
    Connect(m_closePathBtn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&SketchAssistPanel::OnButton_ClosePath);
    Connect(m_continuePathBtn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&SketchAssistPanel::OnButton_ContinuePath);
    Connect(m_clearSketchBtn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&SketchAssistPanel::OnButton_ClearSketch);
    Connect(m_importSketchBtn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&SketchAssistPanel::OnButton_ImportSketch);
    Connect(m_exportSketchBtn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&SketchAssistPanel::OnButton_ExportSketch);

    m_pathsListBox->Connect(wxEVT_LISTBOX, (wxObjectEventFunction)&SketchAssistPanel::OnListBox_PathSelected, nullptr, this);
    m_pathsListBox->Connect(wxEVT_CONTEXT_MENU, (wxObjectEventFunction)&SketchAssistPanel::OnListBox_ContextMenu, nullptr, this);

    m_sketchCanvasPanel->UpdatePathState(SketchCanvasPathState::Undefined);
}

void SketchAssistPanel::SetSketchDef(const std::string& sketchDef)
{
    if ( sketchDef != m_sketchDef) {
        m_sketchDef = sketchDef;
        m_sketch = SketchEffectSketch::SketchFromString(m_sketchDef);
        populatePathListBoxFromSketch();
        Refresh();
    }
}

void SketchAssistPanel::ForwardKeyEvent(wxKeyEvent& event)
{
    if (m_sketchCanvasPanel != nullptr)
        m_sketchCanvasPanel->OnSketchKeyDown(event);
}

SketchEffectSketch& SketchAssistPanel::GetSketch()
{
    return m_sketch;
}

int SketchAssistPanel::GetSelectedPathIndex()
{
    return m_pathsListBox->GetSelection();
}

void SketchAssistPanel::NotifySketchUpdated()
{
    m_sketchDef = m_sketch.toString();
    if (m_sketchUpdateCB != nullptr)
        m_sketchUpdateCB(m_sketchDef);
}

void SketchAssistPanel::NotifySketchPathsUpdated()
{
    populatePathListBoxFromSketch();
}

void SketchAssistPanel::NotifyPathStateUpdated(SketchCanvasPathState state)
{
    switch (state) {
    case SketchCanvasPathState::Undefined:
        m_startPathBtn->Enable();
        m_endPathBtn->Disable();
        m_closePathBtn->Disable();
        m_continuePathBtn->Enable(canContinuePath());
        m_clearSketchBtn->Enable();
        break;
    case SketchCanvasPathState::DefineStartPoint:
        m_startPathBtn->Disable();
        m_endPathBtn->Disable();
        m_closePathBtn->Disable();
        m_continuePathBtn->Disable();
        m_clearSketchBtn->Disable();
        break;
    case SketchCanvasPathState::LineToNewPoint:
    case SketchCanvasPathState::QuadraticCurveToNewPoint:
    case SketchCanvasPathState::CubicCurveToNewPoint:
        m_startPathBtn->Disable();
        m_endPathBtn->Enable();
        m_closePathBtn->Enable();
        m_continuePathBtn->Disable();
        m_clearSketchBtn->Disable();
        break;
    }
}

void SketchAssistPanel::SelectLastPath()
{
    unsigned n;

    if ((n = m_pathsListBox->GetCount()) != 0) {
        m_pathsListBox->SetSelection(n - 1);
        m_continuePathBtn->Enable(canContinuePath());
    }
}

void SketchAssistPanel::UpdateSketchBackground(const wxString& imagePath, int opacity)
{
    if (imagePath == m_bgImagePath && opacity == m_bitmapAlpha)
        return;

    wxImage img(imagePath);
    if (!img.IsOk())
        return;

    if (!img.HasAlpha())
        img.InitAlpha();

    m_bgImagePath = imagePath;
    m_bgImage = img;
    m_bitmapAlpha = static_cast<unsigned char>(opacity);
    updateBgImage();
}

void SketchAssistPanel::OnButton_StartPath(wxCommandEvent& /*event*/)
{
    m_sketchCanvasPanel->ResetHandlesState(SketchCanvasPathState::DefineStartPoint);

    m_pathsListBox->DeselectAll();
}

void SketchAssistPanel::OnButton_EndPath(wxCommandEvent& /*event*/)
{
    m_sketchCanvasPanel->UpdatePathState(SketchCanvasPathState::Undefined);
    NotifySketchUpdated();
}

void SketchAssistPanel::OnButton_ClosePath(wxCommandEvent& /*event*/)
{
    m_sketchCanvasPanel->ClosePath();
    m_sketchCanvasPanel->UpdatePathState(SketchCanvasPathState::Undefined);
    NotifySketchUpdated();
}

void SketchAssistPanel::OnButton_ContinuePath(wxCommandEvent& /*event*/)
{
    m_sketchCanvasPanel->UpdatePathState(SketchCanvasPathState::LineToNewPoint);
}

void SketchAssistPanel::OnButton_ClearSketch(wxCommandEvent& /*event*/)
{
    m_sketch = SketchEffectSketch();

    m_pathsListBox->Clear();

    m_sketchCanvasPanel->ResetHandlesState();
    NotifySketchUpdated();
}

void SketchAssistPanel::OnButton_ImportSketch(wxCommandEvent& event)
{
    wxString filename = wxFileSelector(_("Choose xLights Sketch File"), xLightsFrame::CurrentDir, wxEmptyString, wxEmptyString, "Sketch Files (*.xsketch)|*.xsketch", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (!filename.empty()) {
        wxJSONReader reader;
        wxFile skfile(filename);
        wxString json;
        wxJSONValue data;
        skfile.ReadAll(&json);
        reader.Parse(json, &data);
        skfile.Close();
        if (data.HasMember("sketchdata") && data["sketchdata"].IsString()) {
            SetSketchDef(data["sketchdata"].AsString());
            NotifySketchUpdated();
        }
    }
}

void SketchAssistPanel::OnButton_ExportSketch(wxCommandEvent& event)
{
    wxString filename = wxFileSelector(_("Save xLights Sketch File"), xLightsFrame::CurrentDir, wxEmptyString, wxEmptyString, "Sketch Files (*.xsketch)|*.xsketch", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (!filename.IsEmpty()) {
        wxJSONValue data;
        data["sketchdata"] = m_sketch.toString();
        data["imagepath"] = m_bgImagePath;
        data["bitmapalpha"] = m_bitmapAlpha;
        wxFileOutputStream skfile(filename);
        wxJSONWriter writer(wxJSONWRITER_STYLED, 0, 3);
        writer.Write(data, skfile);
        skfile.Close();
    }    
}

void SketchAssistPanel::OnListBox_PathSelected(wxCommandEvent& /*event*/)
{
    int index = m_pathsListBox->GetSelection();
    m_sketchCanvasPanel->UpdateHandlesForPath(index);
    m_continuePathBtn->Enable(canContinuePath());
}

void SketchAssistPanel::OnListBox_ContextMenu(wxContextMenuEvent& event)
{
    wxPoint pt(m_pathsListBox->ScreenToClient(event.GetPosition()));
    m_pathIndexToDelete = m_pathsListBox->HitTest(pt);
    if (m_pathIndexToDelete < 0)
        return;

    wxString str;

    wxMenu mnu;
    str.sprintf("Reverse Path %d", 1 + m_pathIndexToDelete);
    mnu.Append(ID_MENU_Reverse, str);
    str.sprintf("Delete Path %d", 1 + m_pathIndexToDelete);
    mnu.Append(ID_MENU_Delete, str);
    mnu.Connect(wxEVT_MENU, (wxObjectEventFunction)&SketchAssistPanel::OnPopupCommand, nullptr, this);
    PopupMenu(&mnu);
}

void SketchAssistPanel::OnPopupCommand(wxCommandEvent& event)
{
    if (m_pathIndexToDelete >= m_sketch.pathCount())
        return;
    bool update = false;

    if (event.GetId() == ID_MENU_Delete) {
        m_sketch.deletePath(m_pathIndexToDelete);
        update = true;
    } else if (event.GetId() == ID_MENU_Reverse) {
        m_sketch.reversePath(m_pathIndexToDelete);
        update = true;
    }

    if (update) {
        m_sketchCanvasPanel->ResetHandlesState();

        populatePathListBoxFromSketch();
        NotifySketchUpdated();
    }
}

void SketchAssistPanel::updateBgImage()
{
    if (!m_bgImage.IsOk())
        return;

    int w = m_bgImage.GetWidth();
    int h = m_bgImage.GetHeight();
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
            m_bgImage.SetAlpha(x, y, m_bitmapAlpha);

    m_sketchCanvasPanel->setBackgroundBitmap(std::make_unique<wxBitmap>(m_bgImage));
}

void SketchAssistPanel::populatePathListBoxFromSketch()
{
    m_pathsListBox->Clear();

    for (int i = 0; i < m_sketch.paths().size(); ++i) {
        wxString text;
        text.sprintf("Path %d", i + 1);
        m_pathsListBox->Insert(text, i);
    }
}

bool SketchAssistPanel::canContinuePath() const
{
    int index = m_pathsListBox->GetSelection();
    if (index < 0)
        return false;

    if (index >= m_sketch.pathCount())
        return false;
    auto paths(m_sketch.paths());
    return !paths[index]->isClosed();
}
