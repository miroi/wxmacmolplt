/*****************************************
*  colorArea.cpp
*****************************************/

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "colorArea.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "Globals.h"
#include "wx/wxprec.h"
#include "wx/colordlg.h"

#include "colorArea.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

static const wxColour defaultColor(128,128,128);

BEGIN_EVENT_TABLE(colorArea, wxPanel)
    EVT_MOUSE_EVENTS(colorArea::OnMouse)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(colorPatternArea, wxPanel)
    EVT_MOUSE_EVENTS(colorPatternArea::OnMouse)
    EVT_PAINT  (colorPatternArea::OnPaint)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(patternSelectDlg, wxDialog)
  EVT_BUTTON( wxID_OK, patternSelectDlg::OnOK )
  EVT_BUTTON( wxID_CANCEL, patternSelectDlg::OnCancel )
END_EVENT_TABLE()

  colorArea::colorArea(wxWindow* parent, int id, const RGBColor* color,int w, int h): mWidth(w), mHeight(h), mID(id)
{
  Create(parent, id, wxDefaultPosition, wxSize(mWidth, mHeight), wxSUNKEN_BORDER);

  mCurrentColor = RGB2WX(*color);
  SetBackgroundColour(mCurrentColor);
  
  mPeer = NULL;
}

colorArea::~colorArea()
{
  //delete mDC;
}

void colorArea::draw(void)
{
  SetBackgroundColour(mCurrentColor);

  Update();
}

void colorArea::setColor(const RGBColor * color) {
	mCurrentColor = RGB2WX(*color);
	SetBackgroundColour(mCurrentColor);
	Refresh();
}

void colorArea::setColor(const wxColour* color)
{
  mCurrentColor = *color;
  SetBackgroundColour(mCurrentColor);
  Refresh();
}

const wxColour& colorArea::getColor() const
{
  return mCurrentColor;
}

void colorArea::getColor(RGBColor * color) const {
	RGBColor tmpRGBColor = WX2RGB(mCurrentColor);
	color->red = tmpRGBColor.red;
	color->green = tmpRGBColor.green;
	color->blue = tmpRGBColor.blue;
}

void colorArea::OnMouse(wxMouseEvent &event)
{
	wxColourData clrData;
	clrData.SetColour(GetBackgroundColour());

	if (event.LeftDClick()) {
		wxColourDialog dialog(this, &clrData);
		dialog.SetTitle(_T("Choose Color"));

		if (dialog.ShowModal() == wxID_OK) {
			mCurrentColor = dialog.GetColourData().GetColour();
			SetBackgroundColour(mCurrentColor);

			Refresh();

			if (mPeer)
			  mPeer->setColor(&mCurrentColor);

			wxCommandEvent evt(wxEVT_COMMAND_ENTER, GetId());
			wxPostEvent(this, evt);
		}
	}
}
colorPatternArea::colorPatternArea(wxWindow* parent, int id, const RGBColor* color, int patID, int w, int h)
		: colorArea(parent, id, color, w, h) {

	if ((patID>=0)&&(patID<numPatterns))
		mPattern = new wxBitmap((const char*)atomMaskPatterns[patID], 32, 32);
	else
		mPattern = new wxBitmap((const char*)atomMaskPatterns[0], 32, 32);
	Refresh();

	mParent = parent;
	mPatID = patID;
}

colorPatternArea::~colorPatternArea() {
	delete mPattern;
}

void colorPatternArea::draw() {
	Refresh();
}

void colorPatternArea::reset() {
	SetBackgroundColour(mCurrentColor);
	Refresh();
}

void colorPatternArea::OnMouse(wxMouseEvent &event)
{
  if (event.LeftDClick()) {
    patternSelectDlg selector(this);

    if (selector.ShowModal() == wxOK)
      {

      }
  }

  if ( mID >= ID_BITMAP_SLT && (event.Entering() || mID == dynamic_cast<patternSelectDlg*>(mParent)->getSltId()) )
    SetBackgroundColour(defaultColor);

  if ( event.Leaving() && mID >= ID_BITMAP_SLT && mPatID != dynamic_cast<patternSelectDlg*>(mParent)->getSltPatId() )
    SetBackgroundColour(mCurrentColor);

  if (event.LeftDown() && mID >= ID_BITMAP_SLT)
    {
      dynamic_cast<patternSelectDlg*>(mParent)->setSltId(mID);
    }
}

void colorPatternArea::OnPaint(wxPaintEvent &WXUNUSED(event)) {
	wxPaintDC dc(this);
	PrepareDC(dc);

	dc.SetBrush(wxBrush(*mPattern));
	//dc.SetBrush(wxBrush(*wxRED));
	dc.DrawRectangle(0, 0, mWidth, mHeight);
	//dc.DrawBitmap(*mPattern, 0, 0);
}

patternSelectDlg::patternSelectDlg(colorPatternArea * parent, wxWindowID id, const wxString& caption) {
  mSltPatId = parent->getPatID();
  Create(parent, id, caption);
}

void patternSelectDlg::Create(colorPatternArea * parent, wxWindowID id, const wxString& caption ) {
  wxDialog::Create( parent, id, caption, wxDefaultPosition, wxSize(250, 500), wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX);

  mainSizer = new wxBoxSizer(wxVERTICAL);
  upperSizer = new wxGridSizer(3, numPatterns/3+1);
  lowerSizer = new wxBoxSizer(wxHORIZONTAL);

  RGBColor tmpColor;
  for (int i = 0; i < numPatterns; i++)
    {
      if ( i == parent->getPatID())
	tmpColor = WX2RGB(defaultColor);
      else
	tmpColor = WX2RGB(parent->getColor());

      patSlt[i] = new colorPatternArea(this, ID_BITMAP_SLT+i, &WX2RGB(parent->getColor()), i, 64, 64);
      if ( i == parent->getPatID())
	patSlt[i]->SetBackgroundColour(defaultColor);
      //if the pattern is the one already has been set, use another color

      upperSizer->Add(patSlt[i], 0, wxALIGN_CENTRE | wxALL, 10);
    }

  mButtOK = new wxButton(this, wxID_OK, wxT("OK") );
  mButtCancel = new wxButton(this, wxID_CANCEL, wxT("Cancel"));

  lowerSizer->Add(10,10);
  lowerSizer->Add(mButtOK, 0, wxALIGN_CENTRE | wxALL, 10);
  lowerSizer->Add(mButtCancel, 0, wxALIGN_CENTRE | wxALL, 10);

  mainSizer->Add(upperSizer);
  mainSizer->Add(lowerSizer);

  mainSizer->Layout();
  SetSizer(mainSizer);
  Centre(wxBOTH);
}

patternSelectDlg::~patternSelectDlg()
{
  for ( int i = 0; i < numPatterns; i++)
    delete patSlt[i];
}

void patternSelectDlg::OnOK( wxCommandEvent& WXUNUSED(event) )
{
  std::cout<<mSltId<<"\n";
  Close();
}

void patternSelectDlg::OnCancel( wxCommandEvent& WXUNUSED(event) )
{
  Destroy();
}

void patternSelectDlg::setSltId(int id)
{
  mSltId = id;
  mSltPatId = id - ID_BITMAP_SLT;

  for (int i = 0; i < numPatterns; i++)
    patSlt[i]->reset();
}
