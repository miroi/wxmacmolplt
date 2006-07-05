/////////////////////////////////////////////////////////////////////////////
// Name:        orbital3d.cpp
// Purpose:     
// Author:      Brett Bode
// Modified by: 
// Created:     Fri  2 Jun 10:08:18 2006
// RCS-ID:      
// Copyright:   (c) 2006 Iowa State University
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "surfaceDlg.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "Globals.h"
#include "SurfaceTypes.h"
#include "surfaceDlg.h"
#include "colorArea.h"
#include "MoleculeData.h"
#include "Frame.h"
#include "BasisSet.h"
#include "Prefs.h"
#include "Progress.h"
#include "MolDisplayWin.h"
#include "surfaceswindow.h"

#include <wx/config.h>

////@begin XPM images
////@end XPM images

/*!
 * Orbital3D event table definition
 */

/*BEGIN_EVENT_TABLE( Orbital3DSurf, wxPanel )
    EVT_CHOICE( ID_CHOICE1, Orbital3DSurf::OnChoice1Selected )
    EVT_CHECKBOX( ID_CHECKBOX5, Orbital3DSurf::OnCheckbox5Click )
END_EVENT_TABLE()
*/

using namespace std;

IMPLEMENT_CLASS( BaseSurfacePane, wxPanel )
IMPLEMENT_CLASS( Surface2DPane, wxPanel )
IMPLEMENT_CLASS( Surface3DPane, wxPanel )
IMPLEMENT_CLASS( Orbital2DSurfPane, wxPanel )
IMPLEMENT_CLASS( Orbital3DSurfPane, wxPanel )
IMPLEMENT_CLASS( General3DSurfPane, wxPanel )

IMPLEMENT_CLASS( Surface3DParamDlg, wxFrame )

BEGIN_EVENT_TABLE( Orbital3DSurfPane, wxPanel )
  EVT_CHOICE  (ID_ORB_FORMAT_CHOICE,  Orbital3DSurfPane::OnOrbFormatChoice)
  EVT_CHOICE  (ID_ORB_CHOICE, Orbital3DSurfPane::OnOrbSetChoice)
  EVT_LISTBOX (ID_ATOM_LIST, Orbital3DSurfPane::OnAtomList)
  EVT_CHECKBOX (ID_SPH_HARMONICS_CHECKBOX, Orbital3DSurfPane::OnSphHarmonicChk)
  EVT_RADIOBOX (ID_3D_RADIOBOX, Surface3DPane::On3DRadioBox)
  EVT_CHECKBOX (ID_SMOOTH_CHECKBOX, Surface3DPane::OnSmoothCheck)
  EVT_CHECKBOX (ID_REVERSE_PHASE_CHECKBOX, Orbital3DSurfPane::OnReversePhase)
  EVT_SLIDER (ID_CONTOUR_VALUE_SLIDER, Orbital3DSurfPane::OnContourValueSld)
  EVT_SLIDER (ID_GRID_SIZE_SLIDER, Orbital3DSurfPane::OnGridSizeSld)
  EVT_SLIDER (ID_GRID_POINT_SLIDER, BaseSurfacePane::OnGridPointSld)
  EVT_BUTTON (ID_SURFACE_UPDATE_BUT, Orbital3DSurfPane::OnUpdate)
  EVT_TEXT_ENTER (ID_3D_ORB_TEXTCTRL, Surface3DPane::OnTextEnter)
  EVT_BUTTON (ID_SET_PARAM_BUT, BaseSurfacePane::OnSetParam)
  EVT_BUTTON (ID_FREE_MEM_BUT, Surface3DPane::OnFreeMem)
  EVT_BUTTON (ID_SURFACE_EXPORT_BUT, BaseSurfacePane::OnExport)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE( Surface3DParamDlg, wxFrame )
  EVT_BUTTON (wxID_OK, Surface3DParamDlg::OnClose)
  EVT_BUTTON (wxID_CANCEL, Surface3DParamDlg::OnCancel)
  EVT_BUTTON (ID_COPY_ALL, Surface3DParamDlg::OnCopyAll)
  EVT_BUTTON (ID_PASTE_ALL, Surface3DParamDlg::OnPasteAll)
END_EVENT_TABLE()
/*!
 * Base class of any Panel
 */

BaseSurfacePane::BaseSurfacePane( wxWindow* parent, Surface* target, SurfacesWindow* p, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
	mTarget = target;
	owner = p;
  //mData = data;

	Visible = target->GetVisibility();
	AllFrames = (mTarget->GetSurfaceID() != 0);
	UpdateTest = false;

	Create(parent, id, pos, size, style);
}

BaseSurfacePane::~BaseSurfacePane()
{

}

bool BaseSurfacePane::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
  wxPanel::Create( parent, id, pos, size, style );

  CreateControls();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
  Centre();

  return true;
}

/*
 * In base classes we just create objects
 * leave the adding operation to the final classes
 */

void BaseSurfacePane::CreateControls()
{
  mainSizer = new wxBoxSizer(wxVERTICAL);
  upperSizer = new wxBoxSizer(wxHORIZONTAL);
  middleSizer = new wxBoxSizer(wxHORIZONTAL);
  bottomSizer = new wxBoxSizer(wxHORIZONTAL);
  leftMiddleSizer = new wxBoxSizer(wxVERTICAL);
  upperLeftMiddleSizer = new wxBoxSizer(wxHORIZONTAL);
  lowerLeftMiddleSizer = new wxBoxSizer(wxHORIZONTAL);
  rightMiddleSizer = new wxFlexGridSizer(2,2,0,0);
  leftBottomSizer = new wxBoxSizer(wxHORIZONTAL);
  rightBottomSizer = new wxBoxSizer(wxVERTICAL);

  label0 = new wxStaticText( this, wxID_ANY,
                            _T("Select Orbital Set:"),
                            wxDefaultPosition,
                            wxDefaultSize);

  label1 = new wxStaticText( this, wxID_ANY,
                            _T("Number of\n Grid Points:"),
                            wxDefaultPosition,
                            wxDefaultSize);

  mNumGridPntSld = new wxSlider( this, ID_GRID_POINT_SLIDER, 
				 0, 10, 150,
				 wxDefaultPosition, wxSize(155,wxDefaultCoord),
                             wxSL_AUTOTICKS | wxSL_LABELS);
  //set the initial value in the child object

  mSetParamBut = new wxButton( this, ID_SET_PARAM_BUT, wxT("Set Parameters"), wxPoint(450, 160) );
  mExportBut = new wxButton( this, ID_SURFACE_EXPORT_BUT, wxT("Export"), wxPoint(450, 160) );
  mUpdateBut = new wxButton( this, ID_SURFACE_UPDATE_BUT, wxT("Update"), wxPoint(450, 160) );

  SetSizer(mainSizer);
}

void BaseSurfacePane::OnGridPointSld( wxCommandEvent &event )
{
  NumGridPoints = mNumGridPntSld->GetValue();
  SwitchFixGrid = true;

  setUpdateButton();
}

void BaseSurfacePane::OnSetParam( wxCommandEvent &event )
{
  Surface3DParamDlg* paramDlg = new Surface3DParamDlg(this, mTarget);
  paramDlg->Show();
}

void BaseSurfacePane::OnExport( wxCommandEvent &event )
{
  wxFileDialog dialog(this,_T("Export: wxMacMolPlt"),
		      wxEmptyString, _T("Untitled"),wxEmptyString,
		      wxSAVE|wxOVERWRITE_PROMPT);

  if (dialog.ShowModal() == wxID_OK)
    {
        wxLogMessage(_T("%s"),
                     dialog.GetPath().c_str());
    }

}

//Call to change the visibilty for the active surface
void BaseSurfacePane::SetVisibility(bool state) 
{
  Visible = state;

  setUpdateButton();
}

void BaseSurfacePane::SetUpdateTest(bool test) 
{
  UpdateTest = test;
	
  if (test)
    mUpdateBut->Enable();
  else
    mUpdateBut->Disable();

}

void BaseSurfacePane::setUpdateButton()
{
  if (UpdateNeeded())
    mUpdateBut->Enable();
  else
    mUpdateBut->Disable();
}

bool BaseSurfacePane::UpdateNeeded(void) {return false;}	//By default update is unavailable

OrbSurfacePane::OrbSurfacePane( OrbSurfBase* target, SurfacesWindow* o)
{
  mTarget = target;
  myowner = o;

  //initialize parameters
  TargetSet = mTarget->GetTargetSet();
  OrbOptions = mTarget->GetOptions();
  OrbColumnEnergyOrOccupation = mTarget->GetOrbOccDisplay();
  PlotOrb = target->GetTargetOrb();
  SphericalHarmonics = target->UseSphericalHarmonics();
  PhaseChange = target->GetPhaseChange();

  if ((TargetSet<0)||(!(OrbOptions&1))) 
    {	//default to something sensible
      MoleculeData * mData = myowner->GetMoleculeData();
      Frame * lFrame = mData->GetCurrentFramePtr();
      const std::vector<OrbitalRec *> * Orbs = lFrame->GetOrbitalSetVector();

      if (Orbs->size() > 0) 
	{
	  TargetSet = 0;	//just default to the first set...
	} 
      else
	OrbOptions = 1;	//No MO's so default to AO's
    }

}

OrbSurfacePane::~OrbSurfacePane()
{

}

bool OrbSurfacePane::UpdateEvent() 
{	//user clicked update button, so push the data to the surface

	bool updateGrid = false;
	if (PlotOrb != mTarget->GetTargetOrb()) 
	  {
	    mTarget->SetTargetOrb(PlotOrb);
	    updateGrid = true;
	  }
	if (TargetSet != mTarget->GetTargetSet()) 
	  {
	    mTarget->SetTargetSet(TargetSet);
	    updateGrid = true;
	  }
	if (OrbOptions != mTarget->GetOptions()) 
	  {
	    mTarget->SetOptions(OrbOptions);
	    updateGrid = true;
	  }
	if (OrbColumnEnergyOrOccupation != mTarget->GetOrbOccDisplay()) 
	  {
			//This is just a display flag for the MO column so no
			//grid or contour updates are needed.
	    mTarget->SetOrbOccDisplay(OrbColumnEnergyOrOccupation);
	  }
	if (SphericalHarmonics != mTarget->UseSphericalHarmonics()) 
	  {
	    mTarget->UseSphericalHarmonics(SphericalHarmonics);
	    updateGrid = true;
	  }

	return updateGrid;
}

int OrbSurfacePane::getOrbSetForOrbPane(vector<wxString>& choice)
{
	MoleculeData * mData = myowner->GetMoleculeData();
	Frame * lFrame = mData->GetCurrentFramePtr();
	const vector<OrbitalRec *> * Orbs = lFrame->GetOrbitalSetVector();

  short item, numitems;

  choice.push_back(_T("Atomic Orbitals"));
  item = 1; numitems = 1;

  if (Orbs->size() > 0) 
    {
      std::vector<OrbitalRec *>::const_iterator OrbSet = Orbs->begin();
      long	OrbSetCount = 0;
      while (OrbSet != Orbs->end()) 
	{
	  if (TargetSet == OrbSetCount) item = numitems + 1;

	  if (((*OrbSet)->getOrbitalWavefunctionType() == UHF)&&
	      (!((*OrbSet)->getOrbitalType() == NaturalOrbital))) 
	    {
	      if ((*OrbSet)->getOrbitalType() == LocalizedOrbital) 
		{
		  choice.push_back(_T("Alpha Spin Localized Orbitals"));
		  choice.push_back(_T("Beta Spin Localized Orbitals"));
		} 
	      else if ((*OrbSet)->getOrbitalType() == LocalizedOrbital) 
		{
		  choice.push_back(_T("Alpha Spin Oriented Localized Orbitals"));
		  choice.push_back(_T("Beta Spin Oriented Localized Orbitals"));
		} 
	      else if ((*OrbSet)->getOrbitalType() == GuessOrbital) 
		{
		  choice.push_back(_T("Alpha Spin Initial Guess Orbitals"));
		  choice.push_back(_T("Beta Spin Initial Guess Orbitals"));
		} 
	      else 
		{
		  choice.push_back(_T("Alpha Spin Orbitals"));
		  choice.push_back(_T("Beta Spin Orbitals"));
		}
	      numitems++;	//increment count for two sets, other increment is done below
				
	      if ((TargetSet == OrbSetCount)&&(OrbOptions & 16)) 
		item = numitems+1;	//target beta set
	    } 
	  else 
	    {
	      switch ((*OrbSet)->getOrbitalType()) 
		{
		case OptimizedOrbital:
		  if ((*OrbSet)->getOrbitalWavefunctionType() == MCSCF)
		    choice.push_back(_T("MCSCF Optimized Orbitals"));
		  else
		    choice.push_back(_T("Molecular EigenVectors"));
		  break;

		case NaturalOrbital:
		  switch ((*OrbSet)->getOrbitalWavefunctionType()) 
		    {
		    case UHF:
		      choice.push_back(_T("UHF Natural Orbitals"));
		      break;
		    case GVB:
		      choice.push_back(_T("GVB GI Orbitals"));
		      break;
		    case MCSCF:
		      choice.push_back(_T("MCSCF Natural Orbitals"));
		      break;
		    case CI:
		      choice.push_back(_T("CI Natural Orbitals"));
		      break;
		    case RHFMP2:
		      choice.push_back(_T("RMP2 Natural Orbitals"));
		      break;
		    default:
		      choice.push_back(_T("Natural Orbitals"));
		    }
		  break;

		case LocalizedOrbital:
		  choice.push_back(_T("Localized Orbitals"));
		  break;
		case OrientedLocalizedOrbital:
		  choice.push_back(_T("Oriented Localized Orbitals"));
		  break;
		case GuessOrbital:
		  choice.push_back(_T("Initial Guess Orbitals"));
		  break;
		default:
		  choice.push_back(_T("Molecular Orbitals"));
		}
	    }
	  numitems++;
	  OrbSetCount++;
	  OrbSet++;
	}
	}

  return item;
}

void OrbSurfacePane::makeMOList(vector<wxString>& choice)
{
	MoleculeData * mData = myowner->GetMoleculeData();
	Frame * lFrame = mData->GetCurrentFramePtr();
	const std::vector<OrbitalRec *> * Orbs = lFrame->GetOrbitalSetVector();

  wxString tmpStr;

  if (lFrame && (Orbs->size() > 0)) 
    {
      OrbitalRec * lMOs = NULL;

      if ((TargetSet < Orbs->size())&&(TargetSet >= 0)) 
	lMOs = (*Orbs)[TargetSet];

      if (lMOs) 
	{
	  Boolean Alpha = !(OrbOptions & 16);
	  long	NumMOs;
	  char * SymLabel;
	  float * Energy;
	  float * OccNum;

	  if (Alpha) 
	    {
	      NumMOs=lMOs->NumAlphaOrbs;
	      SymLabel = lMOs->SymType;
	      Energy = lMOs->Energy;
	      OccNum = lMOs->OrbOccupation;
	    } 
	  else 
	    {
	      NumMOs=lMOs->NumBetaOrbs;
	      SymLabel = lMOs->SymTypeB;
	      Energy = lMOs->EnergyB;
	      OccNum = lMOs->OrbOccupationB;
	    }

	  char* oneSymLabel;

	  for (int theCell = 0; theCell < NumMOs; theCell++) 
	    {
	      tmpStr.Printf("%d  ", theCell+1); 
	      char text[30];
	      int nchar;

	      if (SymLabel) 
	      {	//Add the symetry of the orb, if known
		
		oneSymLabel = &(SymLabel[theCell*5]);	
		//offset to the label for this orb

		for (int ichar=0; ichar<4; ichar++) 
		  {
		    if ((oneSymLabel[ichar])=='\0') break;

		    if ((oneSymLabel[ichar]>='0')&&(oneSymLabel[ichar]<='9')) 
		      {
			tmpStr.Append(oneSymLabel[ichar]);
		      } 
		    else if ((oneSymLabel[ichar]=='U')||(oneSymLabel[ichar]=='u')) 
		      {
			tmpStr.Append('u');
		      } 
		    else if ((oneSymLabel[ichar]=='G')||(oneSymLabel[ichar]=='g')) 
		      {
			tmpStr.Append('g');
		      } 
		    else
		      tmpStr.Append(oneSymLabel[ichar]);
		  }
		//TextSize(12);	//reset the point size
	      }

	      tmpStr.Append('\t');

	      nchar=0;
	      if (OrbColumnEnergyOrOccupation) 
		{	//orb occupation selected
		
		  if (OccNum) 
		    sprintf(text, "%.3f%n", OccNum[theCell], &nchar);
		  else 
		    {	
		    //attempt to set the occupation based on the wavefunction type
		    if (lMOs->getOrbitalWavefunctionType() == RHF) 
		      {
			if (theCell<lMOs->getNumOccupiedAlphaOrbitals()) 
			  strcpy(text, "2  ");
			else 
			  strcpy(text, "0  ");
			nchar = 3;
		      } 
		    else if (lMOs->getOrbitalWavefunctionType() == ROHF) 
		      {
			strcpy(text, "0  ");
			if (theCell<lMOs->getNumOccupiedBetaOrbitals()) 
			  strcpy(text, "2  ");
			else if (theCell<lMOs->getNumOccupiedAlphaOrbitals()) 
			  strcpy(text, "1  ");
			nchar = 3;
		      } 
		    else if (lMOs->getOrbitalWavefunctionType() == UHF) 
		      {
			strcpy(text, "0  ");
			if (Alpha) 
			  {
			    if (theCell<lMOs->getNumOccupiedAlphaOrbitals()) 
			      strcpy(text,"1  ");
			  }
			else if (theCell<lMOs->getNumOccupiedBetaOrbitals()) 
			  strcpy(text, "1  ");
			nchar = 3;
		      } 
		    else 
		      {	//MCSCF or CI occupations can't be guessed
			strcpy(text, "??  ");
			nchar = 4;
		      }
		  }
		} 
	      else if (Energy) 
		{	//punch out the orb energy
		  sprintf(text, "%.3f%n", Energy[theCell], &nchar);
		}

	      if (nchar>0) 
		{
		  tmpStr.Append(text);
		  choice.push_back(tmpStr);
		}
	    }
	}
    }
}

void OrbSurfacePane::makeAOList(wxString& choice)
{
	MoleculeData * mData = myowner->GetMoleculeData();
	BasisSet * BasisPtr = mData->GetBasisSet();

  if (BasisPtr) 
    {
      long NumBasisFuncs = BasisPtr->GetNumBasisFuncs(SphericalHarmonics);

      for (int theCell = 0; theCell < NumBasisFuncs; theCell++) 
	{
	  long ifunc = 0, iatom=0;
	  Frame * lFrame = mData->GetCurrentFramePtr();

	  while (ifunc<=theCell) 
	    {
	      long minshell = BasisPtr->BasisMap[2*iatom];
	      long maxshell = BasisPtr->BasisMap[2*iatom+1];

	      for (long ishell=minshell;ishell<=maxshell;ishell++) 
		{
		  long jfunc = ifunc+(BasisPtr->Shells[ishell]).GetNumFuncs(SphericalHarmonics);
		  if (theCell<jfunc) 
		    {	//Found the right shell, now pick out the right function
		      char label[63];
		      wxString tmpStr;
		      int nchar;
		      //punch out the atom # and symbol if this is the 1st function for this atom

		      if ((ishell==minshell)&&(theCell==ifunc))
			{
			  sprintf(label, "%ld  ", iatom+1);
			  choice.Append(label);

			  WinPrefs * mPrefs = myowner->GetPrefs();
			  if (mPrefs) 
			    {
			      long AtomType = lFrame->GetAtomType(iatom)-1;
			      mPrefs->GetAtomLabel(AtomType, tmpStr);
			      choice.Append(tmpStr);
			    }
			}
		      choice.Append('\t');

		      jfunc = theCell-ifunc;
		      BasisPtr->Shells[ishell].GetLabel(label, jfunc, SphericalHarmonics);
		      nchar = strlen(label);
		      
		      if (nchar>0)
			{	//Make sure there really is something there
			  choice.Append(' ');
			  choice.Append(' ');
			  choice.Append(label);

			  for (long ichar=1; ichar<=nchar; ichar++)
			    {
			      char isExponent = ((label[ichar]>='0')&&
						 (label[ichar]<='9')&&
						 (label[ichar-1]>='r')&&
						 (label[ichar-1]<='z'));
			      if (isExponent)
				{
				      //adjust vertical position
				}
				
				  //DrawText(label, ichar, 1);
			    }
			}
		      choice.Append('\t');

		      ifunc = theCell+1;
		      ishell=maxshell;
		    }
		  else
		    ifunc = jfunc;
		}
	      iatom++;
	    }
	  
	  if (PlotOrb>=0) 
	    {	//Is an MO selected?
	      if (!(OrbOptions&1)) 
		{	//If not displaying AO's
		  const std::vector<OrbitalRec *> * Orbs = lFrame->GetOrbitalSetVector();
		  if (Orbs->size() > 0) 
		    {
		      OrbitalRec *MOs = NULL;
		      if ((TargetSet < Orbs->size())&&(TargetSet >= 0)) 
			MOs = (*Orbs)[TargetSet];

		      if (MOs) 
			{
			  float * aVector=NULL;

			  if (OrbOptions & 16) 
			    {
			      if (PlotOrb<MOs->NumBetaOrbs)
				aVector = &(MOs->VectorsB[NumBasisFuncs*PlotOrb]);
			    } 
			  else 
			    {
			      if (PlotOrb<MOs->NumAlphaOrbs)
				aVector = &(MOs->Vectors[NumBasisFuncs*PlotOrb]);
			    }

			  if (aVector) 
			    {
			      char label[63];

			      sprintf(label, "%.3f", aVector[theCell]);//prepare the coef for printing
			      choice.Append(label);
			    }
			}
		    }
		}
	    }
	  choice.Append('\n');
	}
    }
}

/*
 * General 2D surface dialog class
 */

Surface2DPane::Surface2DPane( wxWindow* parent, Surf2DBase* target, 
			      SurfacesWindow* Owner, wxWindowID id, 
			      const wxPoint& pos, const wxSize& size, 
			      long style ) 
  : BaseSurfacePane(parent, target, Owner, id, pos, size, style)
{
  mTarget = target;
  CreateControls();
}

Surface2DPane::~Surface2DPane()
{
  delete mOrbColor1;
  delete mOrbColor2;
}

void Surface2DPane::CreateControls()
{
  mNumContourLabel = new wxStaticText( this, wxID_ANY,
                            _T("Max # of contours:"),
                            wxDefaultPosition,
                            wxDefaultSize);

  mContourValLabel = new wxStaticText( this, wxID_ANY,
                            _T("Max contour value:"),
                            wxDefaultPosition,
                            wxDefaultSize);

  mNumContourText = new wxTextCtrl( this, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize);
  mContourValText = new wxTextCtrl( this, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize);

  mShowZeroCheck = new wxCheckBox( this, ID_SHOW_ZERO_CHECKBOX, _T("Show zero contour"), wxPoint(340,130), wxDefaultSize );
  mDashCheck = new wxCheckBox( this, ID_DASH_CHECKBOX, _T("Dash - contour"), wxPoint(340,130), wxDefaultSize );

  mSetPlaneBut = new wxButton( this, ID_SET_PLANE_BUT, wxT("Set Plane"));

  mTarget->GetPosColor(&PosColor);
  mTarget->GetNegColor(&NegColor);

  mOrbColor1 = new colorArea(this, ID_2D_COLOR_POSITIVE, &PosColor);
//  mOrbColor1->draw(&PosColor);
  mOrbColor2 = new colorArea(this, ID_2D_COLOR_NEGATIVE, &NegColor);
//  mOrbColor2->draw(&NegColor);
}

/*
 * General 3D surface dialog class
 */

Surface3DPane::Surface3DPane( wxWindow* parent, Surf3DBase* target, 
			      SurfacesWindow* Owner, wxWindowID id,
			      const wxPoint& pos, const wxSize& size, 
			      long style ) 
  : BaseSurfacePane(parent, target, Owner, id, pos, size, style)
{
  mTarget = target;
  CreateControls();
  UseSolidSurface = mTarget->SolidSurface();
  UseNormals = mTarget->UseSurfaceNormals();
}

Surface3DPane::~Surface3DPane()
{
  delete mOrbColor1;
  delete mOrbColor2;
  delete mTransColor;
}

void Surface3DPane::CreateControls()
{
  float GridMax = mTarget->GetGridMax();

  label2 = new wxStaticText( this, wxID_ANY,
                            _T("Grid Size:"),
                            wxDefaultPosition,
                            wxDefaultSize);
  label3 = new wxStaticText( this, wxID_ANY,
                            _T("Contour Value:"),
                            wxDefaultPosition,
                            wxDefaultSize);

  mGridSizeSld = new wxSlider( this, ID_GRID_SIZE_SLIDER, 
			       (short) (100*mTarget->GetGridSize()), 0, 300,
                             wxDefaultPosition, wxSize(155,wxDefaultCoord),
                             wxSL_AUTOTICKS | wxSL_LABELS);
  mContourValSld = new wxSlider( this, ID_CONTOUR_VALUE_SLIDER, 
				 (short)(100*(mTarget->GetContourValue()/((fabs(GridMax)>=0.001)?GridMax:0.25))), 
				 0, 100, wxDefaultPosition, 
				 wxSize(155,wxDefaultCoord));

  m3DOrbMaxText = new wxTextCtrl( this, ID_3D_ORB_TEXTCTRL, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);

  label4 = new wxStaticText( this, wxID_ANY,
                            _T("Transparency Color:"),
                            wxDefaultPosition,
                            wxDefaultSize);

  mTarget->GetTranspColor(&TranspColor);
  mTarget->GetPosColor(&PosColor);
  mTarget->GetNegColor(&NegColor);

  mOrbColor1 = new colorArea(this, ID_3D_COLOR_POSITIVE, &PosColor);
//  mOrbColor1->draw(&PosColor);
  mOrbColor2 = new colorArea(this, ID_3D_COLOR_NEGATIVE, &NegColor);
//  mOrbColor2->draw(&NegColor);
  mTransColor = new colorArea(this, ID_TRANSPARENCY_COLOR, &TranspColor);
//  mTransColor->draw(&TranspColor);

  wxString choices[] = {_T("Solid"), _T("Wire Frame")};
  m3DRdoBox = new wxRadioBox( this, ID_3D_RADIOBOX, _T(""), wxDefaultPosition, wxDefaultSize, WXSIZEOF(choices), choices, 1, wxRA_SPECIFY_ROWS );
  m3DRdoBox->SetSelection(1-UseSolidSurface);
  
  mSmoothChkBox = new wxCheckBox( this, ID_SMOOTH_CHECKBOX, _T("Smooth"), wxPoint(340,130), wxDefaultSize );
  mSmoothChkBox->SetValue(UseNormals);

  if (UseSolidSurface)
    mSmoothChkBox->Enable();
  else
    mSmoothChkBox->Disable();

  mFreeMemBut = new wxButton( this, ID_FREE_MEM_BUT, wxT("Free Mem"), wxPoint(450, 160) );

}

void Surface3DPane::setContourValueSld()
{
  float GridMax = mTarget->GetGridMax();
  mContourValSld->SetValue((short)(100*(ContourValue/((fabs(GridMax)>=0.001)?GridMax:0.25))));

}

void Surface3DPane::On3DRadioBox (wxCommandEvent& event )
{
  UseSolidSurface = 1-m3DRdoBox->GetSelection();

  if (UseSolidSurface)
    mSmoothChkBox->Enable();
  else
    mSmoothChkBox->Disable();

  setUpdateButton();
}

void Surface3DPane::OnSmoothCheck (wxCommandEvent& event )
{
  UseNormals = mSmoothChkBox->GetValue();

  setUpdateButton();
}

void Surface3DPane::OnTextEnter(wxCommandEvent& event )
{
  float newVal;

  if (mTarget->GetGridMax() > 0.000001 )
    {
      wxString tmpStr = m3DOrbMaxText->GetValue();
      newVal = atof(tmpStr.c_str());

      if (newVal < 0.0) newVal *= -1.0;
      if (newVal > mTarget->GetGridMax()) 
	newVal = mTarget->GetGridMax();
      
      ContourValue = newVal;
      tmpStr.Printf("%.4f", newVal);

      setContourValueSld();
      m3DOrbMaxText->SetValue(tmpStr);
    }
}

void Surface3DPane::OnFreeMem(wxCommandEvent& event )
{
  mTarget->FreeGrid();
  mFreeMemBut->Disable();
}

/*!
 * Orbital2D class
 */

Orbital2DSurfPane::Orbital2DSurfPane( wxWindow* parent, Orb2DSurface* target, 
				      SurfacesWindow* o, wxWindowID id, 
				      const wxPoint& pos, const wxSize& size, 
				      long style ) 
  : Surface2DPane(parent, target, o, id, pos, size, style), OrbSurfacePane(target, o)
{
  mTarget = target;

  TargetToPane();
  CreateControls();
}

Orbital2DSurfPane::~Orbital2DSurfPane()
{

}

void Orbital2DSurfPane::CreateControls()
{
  upperSizer->Add(label0, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);

  vector<wxString> choices;
  int itemSelect = getOrbSetForOrbPane(choices) - 1;

  mOrbSetChoice = new wxChoice( this, ID_ORB_CHOICE, wxPoint(10,10), wxSize(200,wxDefaultCoord), choices.size(), &choices.front() );
  mOrbSetChoice->SetSelection(itemSelect);
  upperSizer->Add(mOrbSetChoice, wxALIGN_CENTER_VERTICAL | wxALL, 3);

  mSphHarmonicsChk = new wxCheckBox( this, ID_SPH_HARMONICS_CHECKBOX, _T("Spherical Harmonics"), wxDefaultPosition, wxDefaultSize );

  wxString choices1[] = {_T("Energy"), _T("Occupation #")};
  mOrbFormatChoice = new wxChoice( this, ID_ORB_FORMAT_CHOICE, wxDefaultPosition, wxSize(120,wxDefaultCoord), 2, choices1 );

  vector<wxString> choices2;
  makeMOList(choices2);

  wxString choices3;
  makeAOList(choices3);

  mAtomList = new wxListBox( this, ID_ATOM_LIST,
                             wxDefaultPosition, wxSize(130,180),
                             choices2.size(), &choices2.front(), 
			     wxLB_SINGLE |wxLB_ALWAYS_SB );

  mOrbCoef = new wxTextCtrl( this, wxID_ANY, choices3, wxPoint(20,160), wxSize(120,200), wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL);

  mSphHarmonicsChk->SetValue(SphericalHarmonics);
  upperLeftMiddleSizer->Add(mSphHarmonicsChk, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);

  if (OrbOptions & 1 && PlotOrb >= 0)
    upperLeftMiddleSizer->Show(mSphHarmonicsChk, true);
  else
    upperLeftMiddleSizer->Show(mSphHarmonicsChk, false);

  upperLeftMiddleSizer->Layout();

  lowerLeftMiddleSizer->Add(label1, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);
  lowerLeftMiddleSizer->Add(mNumGridPntSld, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);
  mNumGridPntSld->SetValue(NumGridPoints);

  leftMiddleSizer->Add(upperLeftMiddleSizer, 0, wxALL, 3);
  leftMiddleSizer->Add(lowerLeftMiddleSizer, 0, wxALL, 3);

  rightMiddleSizer->Add(mNumContourLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 10);
  rightMiddleSizer->Add(mNumContourText, 0, wxALIGN_CENTER_VERTICAL | wxALL, 10);
  rightMiddleSizer->Add(mContourValLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 10);
  rightMiddleSizer->Add(mContourValText, 0, wxALIGN_CENTER_VERTICAL | wxALL, 10);

  mSubLeftBot1Sizer = new wxBoxSizer(wxVERTICAL);
  mSubLeftBot2Sizer = new wxBoxSizer(wxVERTICAL);
  
  mSubLeftBot1Sizer->Add(new wxStaticText(this, wxID_ANY,
                            _T("Select Orb:"),
                            wxDefaultPosition,
			    wxDefaultSize), 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);

  mSubLeftBot1Sizer->Add(mOrbFormatChoice, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);

  mSubLeftBot1Sizer->Add(mAtomList, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);

  mSubLeftBot2Sizer->Add(new wxStaticText(this, wxID_ANY,
                         _T("Orbital vector: \nAtom Orbital Coef"),
                         wxDefaultPosition,
                         wxDefaultSize), 0, wxALIGN_CENTER_VERTICAL | wxALL, 1);
  mSubLeftBot2Sizer->Add(mOrbCoef, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);

  leftBottomSizer->Add(mSubLeftBot1Sizer, 0, wxALL, 5);
  leftBottomSizer->Add(mSubLeftBot2Sizer, 0, wxALL, 5);

  mSubRightBot1Sizer = new wxBoxSizer(wxVERTICAL);
  mUsePlaneChk = new wxCheckBox(this, ID_USE_PLANE_CHECKBOX, _T("Use plane of screen"), wxDefaultPosition);
  mRevPhaseChk = new wxCheckBox(this, ID_REVERSE_PHASE_CHECKBOX, _T("Reverse Phase"), wxDefaultPosition);
  mSubRightBot1Sizer->Add(mUsePlaneChk, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);
  mSubRightBot1Sizer->Add(mShowZeroCheck, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);
  mSubRightBot1Sizer->Add(mDashCheck, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);
  mSubRightBot1Sizer->Add(mRevPhaseChk, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);

  mSubRightBot2Sizer = new wxBoxSizer(wxHORIZONTAL);
  mSubRightBot2Sizer->Add(new wxStaticText(this, wxID_ANY,
                            _T("Orbital\n Colors:"),
                            wxDefaultPosition,
			    wxDefaultSize), 0, wxALIGN_CENTER_VERTICAL | wxALL, 8);
  mSubRightBot2Sizer->Add(mOrbColor1, 0, wxALIGN_CENTER_VERTICAL | wxALL, 8);
  mSubRightBot2Sizer->Add(mOrbColor2, 0, wxALIGN_CENTER_VERTICAL | wxALL, 8);

  mSubRightBot3Sizer = new wxGridSizer(2,2,0,0);
  mSubRightBot3Sizer->Add(mSetParamBut, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);
  mSubRightBot3Sizer->Add(mSetPlaneBut, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);
  mSubRightBot3Sizer->Add(mExportBut, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);
  mSubRightBot3Sizer->Add(mUpdateBut, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);

  rightBottomSizer->Add(mSubRightBot1Sizer);
  rightBottomSizer->Add(mSubRightBot2Sizer);
  rightBottomSizer->Add(mSubRightBot3Sizer);

  middleSizer->Add(leftMiddleSizer, 0, wxALL, 10);
  middleSizer->Add(rightMiddleSizer, 0, wxALL, 10);
  bottomSizer->Add(leftBottomSizer, 0, wxALL, 3);
  bottomSizer->Add(rightBottomSizer, 0, wxALL, 3);
  mainSizer->Add(upperSizer);
  mainSizer->Add(middleSizer);
  mainSizer->Add(bottomSizer);
}

void Orbital2DSurfPane::TargetToPane(void) 
{
  NumGridPoints = mTarget->GetNumGridPoints();
  NumContours = mTarget->GetNumContours();
  MaxContourValue = mTarget->GetMaxValue();
  mTarget->GetPosColor(&PosColor);
  mTarget->GetNegColor(&NegColor);
  ShowZeroContour = mTarget->GetShowZeroContour();
  UseScreenPlane = mTarget->GetRotate2DMap();
  Visible = mTarget->GetVisibility();
  DashLines = mTarget->GetDashLine();
  UpdateTest = false;
}

void Orbital2DSurfPane::refreshControls()
{

}

/*!
 * Orbital3D class
 */

Orbital3DSurfPane::Orbital3DSurfPane( wxWindow* parent, Orb3DSurface* target, 
				      SurfacesWindow* o, wxWindowID id,
				      const wxPoint& pos, const wxSize& size, 
				      long style ) 
  : Surface3DPane(parent, target, o, id, pos, size, style), OrbSurfacePane(target, o)
{
  mTarget = target;

  TargetToPane();
  CreateControls();
}

Orbital3DSurfPane::~Orbital3DSurfPane()
{

}

void Orbital3DSurfPane::TargetToPane(void) 
{
  NumGridPoints = mTarget->GetNumGridPoints();
  mTarget->GetPosColor(&PosColor);
  mTarget->GetNegColor(&NegColor);
  mTarget->GetTranspColor(&TranspColor);
  GridSize = mTarget->GetGridSize();
  ContourValue = mTarget->GetContourValue();
  UseSolidSurface = mTarget->SolidSurface();
  UseNormals = mTarget->UseSurfaceNormals();
  UpdateTest = false;
  SwitchFixGrid = false;
}

void Orbital3DSurfPane::refreshControls()
{
  float GridMax = mTarget->GetGridMax();

  mNumGridPntSld->SetValue(NumGridPoints);
  mGridSizeSld->SetValue((short)(100*GridSize));
  mContourValSld->SetValue((short)(100*(ContourValue/((fabs(GridMax)>=0.001)?GridMax:0.25))));
  m3DRdoBox->SetSelection(1-UseSolidSurface);
  mSmoothChkBox->SetValue(UseNormals);

  if (UseSolidSurface)
    mSmoothChkBox->Enable();
  else
    mSmoothChkBox->Disable();

  mOrbColor1->setColor(&PosColor);
  mOrbColor2->setColor(&NegColor);
  mTransColor->setColor(&TranspColor);
//  mOrbColor1->draw(&PosColor);
  //mOrbColor2->draw(&NegColor);
 // mTransColor->draw(&TranspColor);
}

/*!
 * Control creation for Orbital3D
 */

void Orbital3DSurfPane::CreateControls()
{    
  float GridMax = mTarget->GetGridMax();

  upperSizer->Add(label0, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);

  vector<wxString> choices;
  int itemSelect = getOrbSetForOrbPane(choices) - 1;

  mOrbSetChoice = new wxChoice( this, ID_ORB_CHOICE, wxPoint(10,10), wxSize(200,wxDefaultCoord), choices.size(), &choices.front() );
  mOrbSetChoice->SetSelection(itemSelect);
  upperSizer->Add(mOrbSetChoice, wxALIGN_CENTER_VERTICAL | wxALL, 3);

  upperSizer->Add(10,10);

  mRevPhaseChk = new wxCheckBox(this, ID_REVERSE_PHASE_CHECKBOX, _T("Reverse Phase"), wxDefaultPosition);
  mRevPhaseChk->SetValue(PhaseChange);

  upperSizer->Add(mRevPhaseChk, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);

  mSphHarmonicsChk = new wxCheckBox( this, ID_SPH_HARMONICS_CHECKBOX, _T("Spherical Harmonics"), wxDefaultPosition, wxDefaultSize );

  wxString choices1[] = {_T("Energy"), _T("Occupation #")};
  mOrbFormatChoice = new wxChoice( this, ID_ORB_FORMAT_CHOICE, wxDefaultPosition, wxSize(120,wxDefaultCoord), 2, choices1 );

  vector<wxString> choices2;
  makeMOList(choices2);

  wxString choices3;
  makeAOList(choices3);

  mAtomList = new wxListBox( this, ID_ATOM_LIST,
                             wxDefaultPosition, wxSize(130,180),
                             choices2.size(), &choices2.front(), 
			     wxLB_SINGLE |wxLB_ALWAYS_SB );

  mOrbCoef = new wxTextCtrl( this, wxID_ANY, choices3, wxPoint(20,160), wxSize(120,200), wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL);

  mSphHarmonicsChk->SetValue(SphericalHarmonics);
  upperLeftMiddleSizer->Add(mSphHarmonicsChk, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);

  if (OrbOptions & 1 && PlotOrb >= 0)
    upperLeftMiddleSizer->Show(mSphHarmonicsChk, true);
  else
    upperLeftMiddleSizer->Show(mSphHarmonicsChk, false);

  upperLeftMiddleSizer->Layout();

  lowerLeftMiddleSizer->Add(label1, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);
  lowerLeftMiddleSizer->Add(mNumGridPntSld, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);
  mNumGridPntSld->SetValue(NumGridPoints);

  leftMiddleSizer->Add(upperLeftMiddleSizer, 0, wxALL, 3);
  leftMiddleSizer->Add(lowerLeftMiddleSizer, 0, wxALL, 3);

  rightMiddleSizer->Add(label2, 0, wxALIGN_CENTER_VERTICAL | wxALL, 10);
  rightMiddleSizer->Add(mGridSizeSld, 0, wxALIGN_CENTER_VERTICAL | wxALL, 10);
  rightMiddleSizer->Add(label3, 0, wxALIGN_CENTER_VERTICAL | wxALL, 10);
  rightMiddleSizer->Add(mContourValSld, 0, wxALIGN_CENTER_VERTICAL | wxALL, 10);

  mSubLeftBot1Sizer = new wxBoxSizer(wxVERTICAL);
  mSubLeftBot2Sizer = new wxBoxSizer(wxVERTICAL);
  
  mSubLeftBot1Sizer->Add(new wxStaticText(this, wxID_ANY,
                            _T("Select Orb:"),
                            wxDefaultPosition,
			    wxDefaultSize), 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);

  mSubLeftBot1Sizer->Add(mOrbFormatChoice, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);

  mSubLeftBot1Sizer->Add(mAtomList, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);

  mSubLeftBot2Sizer->Add(new wxStaticText(this, wxID_ANY,
                         _T("Orbital vector: \nAtom Orbital Coef"),
                         wxDefaultPosition,
                         wxDefaultSize), 0, wxALIGN_CENTER_VERTICAL | wxALL, 1);
  mSubLeftBot2Sizer->Add(mOrbCoef, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);

  leftBottomSizer->Add(mSubLeftBot1Sizer, 0, wxALL, 5);
  leftBottomSizer->Add(mSubLeftBot2Sizer, 0, wxALL, 5);

  mSubRightBot0Sizer = new wxBoxSizer(wxHORIZONTAL);
  mSubRightBot0Sizer->Add(m3DOrbMaxText);

  wxString tmpStr;
  tmpStr.Printf("%.4f", ContourValue);
  m3DOrbMaxText->SetValue(tmpStr);

  mSubRightBot0Sizer->Add(30,30);

  mSubRightBot0Sizer->Add(new wxStaticText(this, wxID_ANY,
                          _T("0"), wxDefaultPosition, wxDefaultSize), 
			  0, wxALIGN_TOP | wxALL, 3);
  mSubRightBot0Sizer->Add(100,30);

  tmpStr.Printf("%.4f", GridMax);
  mContourMaxTick = new wxStaticText(this, wxID_ANY, tmpStr, 
				     wxDefaultPosition, wxDefaultSize);
  mSubRightBot0Sizer->Add(mContourMaxTick, 
		          0, wxALIGN_TOP | wxALL, 3);

  mSubRightBot1Sizer = new wxBoxSizer(wxHORIZONTAL);
  mSubRightBot1Sizer->Add(new wxStaticText(this, wxID_ANY,
                          _T("Orbital Colors:"),
                          wxDefaultPosition,
                          wxDefaultSize), 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);

  mSubRightBot1Sizer->Add(mOrbColor1, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);
  mSubRightBot1Sizer->Add(mOrbColor2, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);

  mSubRightBot2Sizer = new wxBoxSizer(wxHORIZONTAL);
  mSubRightBot2Sizer->Add(label4, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);
  mSubRightBot2Sizer->Add(mTransColor, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);

  mSubRightBot3Sizer = new wxBoxSizer(wxHORIZONTAL);
  mSubRightBot3Sizer->Add(m3DRdoBox, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);

  mSubRightBot4Sizer = new wxBoxSizer(wxHORIZONTAL);
  mSubRightBot4Sizer->Add(mSmoothChkBox,0, wxALIGN_CENTER_VERTICAL | wxALL, 3);

  mSubRightBot5Sizer = new wxGridSizer(2,2,0,0);
  mSubRightBot5Sizer->Add(mSetParamBut, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);
  mSubRightBot5Sizer->Add(mExportBut, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);
  mSubRightBot5Sizer->Add(mFreeMemBut, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);
  mSubRightBot5Sizer->Add(mUpdateBut, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);

  rightBottomSizer->Add(mSubRightBot0Sizer);
  rightBottomSizer->Add(20, 20);
  rightBottomSizer->Add(mSubRightBot1Sizer);
  rightBottomSizer->Add(mSubRightBot2Sizer);
  rightBottomSizer->Add(mSubRightBot3Sizer);
  rightBottomSizer->Add(mSubRightBot4Sizer);
  rightBottomSizer->Add(mSubRightBot5Sizer);

  middleSizer->Add(leftMiddleSizer, 0, wxALL, 10);
  middleSizer->Add(rightMiddleSizer, 0, wxALL, 10);
  bottomSizer->Add(leftBottomSizer, 0, wxALL, 3);
  bottomSizer->Add(rightBottomSizer, 0, wxALL, 3);
  mainSizer->Add(upperSizer);
  mainSizer->Add(middleSizer);
  mainSizer->Add(bottomSizer);
}

bool Orbital3DSurfPane::UpdateNeeded(void) 
{
  bool result = false;

  if (PlotOrb >= 0) 
    {	//Don't update unless a valid orbital is chosen
      if (PlotOrb != mTarget->GetTargetOrb()) result=true;
      if (Visible != mTarget->GetVisibility()) result = true;
      if (AllFrames != (mTarget->GetSurfaceID() != 0)) result = true;
      if (TargetSet != mTarget->GetTargetSet()) result = true;
      if (OrbOptions != mTarget->GetOptions()) result = true;
      if (NumGridPoints != mTarget->GetNumGridPoints()) result = true;
      if (ContourValue != mTarget->GetContourValue()) result = true;
      if (GridSize != mTarget->GetGridSize()) result = true;
      if (UseSolidSurface != mTarget->SolidSurface()) result = true;
      if (UseNormals != mTarget->UseSurfaceNormals()) result = true;
      if (PhaseChange != mTarget->GetPhaseChange()) result = true;
      if (SphericalHarmonics != mTarget->UseSphericalHarmonics()) result = true;

      if (!result) 
	{
	  RGBColor	testColor;
	  mTarget->GetPosColor(&testColor);
	  if ((PosColor.red != testColor.red)||(PosColor.green!=testColor.green)
	      ||(PosColor.blue!=testColor.blue)) 
	    result=true;

	  mTarget->GetNegColor(&testColor);
	  if ((NegColor.red != testColor.red)||(NegColor.green!=testColor.green)
	      ||(NegColor.blue!=testColor.blue)) 
	    result=true;

	  mTarget->GetTranspColor(&testColor);
	  if ((TranspColor.red != testColor.red)
	      ||(TranspColor.green!=testColor.green)
	      ||(TranspColor.blue!=testColor.blue)) 
	    result=true;
	}
    }
  return result;
}

void Orbital3DSurfPane::OnOrbFormatChoice( wxCommandEvent &event )
{
  int itemtype = mOrbFormatChoice->GetSelection();

  if (OrbColumnEnergyOrOccupation) OrbColumnEnergyOrOccupation = false;
  if (itemtype) OrbColumnEnergyOrOccupation = true;

  vector<wxString> newChoice;
  makeMOList(newChoice);

  mAtomList->Set(newChoice.size(), &newChoice.front());

}

void Orbital3DSurfPane::OnOrbSetChoice( wxCommandEvent &event )
{
  int item = mOrbSetChoice->GetSelection()+1;
  int itemtype=0;

  if (item <= 1) 
    {
      if (!(OrbOptions&1)) 
	{
	  OrbOptions = 1; itemtype=1;
	}
    } 
  else 
    {
      short numitems=1;
      OrbOptions = 0;	//turn off AOs and alpha/beta flags
      MoleculeData * mData = owner->GetMoleculeData();
      Frame * lFrame = mData->GetCurrentFramePtr();

      const std::vector<OrbitalRec *> * Orbs = lFrame->GetOrbitalSetVector();

      if (Orbs->size() > 0) 
	{
	  vector<OrbitalRec *>::const_iterator OrbSet = Orbs->begin();
	  long	OrbSetCount = 0;

	  while (OrbSet != Orbs->end()) 
	    {
	      numitems++;
	      if (numitems == item) 
		{
		  TargetSet = OrbSetCount;
		  itemtype = 1;
		  break;
		}

	      if (((*OrbSet)->getOrbitalWavefunctionType() == UHF)&&
		  (!((*OrbSet)->getOrbitalType() == NaturalOrbital))) 
		{
		  numitems++;	//Extra increment for the beta set
		  if (numitems == item) 
		    {
		      TargetSet = OrbSetCount;
		      OrbOptions = 16;	//beta set selected
		      itemtype = 1;
		      break;
		    }
		}
	      OrbSetCount++;
	      OrbSet++;
	    }
	}
    }		
			
  if (itemtype) 
    {	//TargetSet has been choosen
      PlotOrb = -1;

      //BasisSet * lBasis = MainData->GetBasisSet();
      //LAddRow(lBasis->GetNumBasisFuncs(SphericalHarmonics), 0, AOList);

      if (!(OrbOptions&1)) 
	{	//add some MO rows!
	  //don't know what's this adding rows for  -Song

	  upperLeftMiddleSizer->Show(mSphHarmonicsChk, false);	  
	  SphericalHarmonics = false;
	} 
      else 
	{	//Must be looking for AOs
	  upperLeftMiddleSizer->Show(mSphHarmonicsChk, true);
	  mSphHarmonicsChk->SetValue(SphericalHarmonics);
	}

      upperLeftMiddleSizer->Layout();
    }

  if (item <= 1)
    {
      mAtomList->Clear();
      PlotOrb = -1;

      wxString choice;
      makeAOList(choice);
      mOrbCoef->SetValue(choice);
    }
  else
    {
      vector<wxString> newChoice;
      makeMOList(newChoice);

      mAtomList->Set(newChoice.size(), &newChoice.front());
    }

}

void Orbital3DSurfPane::OnAtomList( wxCommandEvent &event )
{
  PlotOrb = event.GetSelection();
  
  wxString choice;
  makeAOList(choice);
  mOrbCoef->SetValue(choice);

  setUpdateButton();  
}

void Orbital3DSurfPane::OnSphHarmonicChk(wxCommandEvent &event )
{
  SphericalHarmonics = mSphHarmonicsChk->GetValue();
}

void Orbital3DSurfPane::OnReversePhase(wxCommandEvent &event )
{
  PhaseChange = mRevPhaseChk->GetValue();

  setUpdateButton();
}

void Orbital3DSurfPane::OnContourValueSld(wxCommandEvent &event )
{
  float GridMax = mTarget->GetGridMax();
  ContourValue = 0.01 * mContourValSld->GetValue() * ((fabs(GridMax)>=0.001)?GridMax:0.25);
  //mTarget->SetContourValue(ContourValue);

  wxString tmpStr;
  tmpStr.Printf("%.4f", ContourValue);
  m3DOrbMaxText->SetValue(tmpStr);

  setUpdateButton();
}

void Orbital3DSurfPane::OnGridSizeSld(wxCommandEvent &event )
{
  GridSize = 0.01 * mGridSizeSld->GetValue();
  SwitchFixGrid = true;

  setUpdateButton();
}

void Orbital3DSurfPane::OnUpdate(wxCommandEvent &event )
{
  bool updateGrid=UpdateTest;
  bool updateContour=false;

  if (PlotOrb >= 0) 
    {	//Don't update unless a valid orbital is chosen
      if (PlotOrb != mTarget->GetTargetOrb()) updateGrid=true;
      if (SphericalHarmonics != mTarget->UseSphericalHarmonics()) updateGrid = true;
      if (OrbOptions != mTarget->GetOptions()) updateGrid = true;
      if (NumGridPoints != mTarget->GetNumGridPoints()) updateGrid = true;
      if (ContourValue != mTarget->GetContourValue()) updateContour = true;
      if (GridSize != mTarget->GetGridSize()) updateGrid = true;
      if (PhaseChange != mTarget->GetPhaseChange()) updateGrid = true;
    }

  if (SwitchFixGrid) 
    {
      mTarget->SetFixGrid(false);
      SwitchFixGrid = false;
      updateGrid = true;
    }

  if (Visible && !mTarget->ContourAvail()) updateContour = true;
  //test to see if grid or contour must calculated anyway
  if (updateContour && ! mTarget->GridAvailable()) updateGrid = true;
  if (updateGrid) updateContour = true;

  mTarget->SetVisibility(Visible);
  mTarget->SolidSurface(UseSolidSurface);
  mTarget->SetNumGridPoints(NumGridPoints);
  mTarget->SetContourValue(ContourValue);
  mTarget->SetGridSize(GridSize);
  mTarget->SetPosColor(&PosColor);
  mTarget->SetNegColor(&NegColor);
  mTarget->SetTranspColor(&TranspColor);
  mTarget->SetPhaseChange(PhaseChange);
  mTarget->UseSurfaceNormals(UseNormals);

  OrbSurfacePane::UpdateEvent();
  
  MoleculeData * mData = owner->GetMoleculeData();

  //update this surface's data on all frames if necessary
  if (AllFrames != (mTarget->GetSurfaceID() != 0)) 
    {	//update all frames
      long	SurfaceID;
      Frame *	lFrame = mData->GetFirstFrame();
      updateGrid = updateContour = true;
      if (AllFrames) 
	{	//adding the surface to all frames
	  SurfaceID = mTarget->SetSurfaceID();
	  while (lFrame) 
	    {
	      if (lFrame != mData->GetCurrentFramePtr()) 
		{
		  Orb3DSurface * NewSurface = new Orb3DSurface(mTarget);
		  lFrame->AppendSurface(NewSurface);
		}
	      lFrame = lFrame->GetNextFrame();
	    }
	} 
      else 
	{			//deleting the surface from other frames
	  SurfaceID = mTarget->GetSurfaceID();
	  mTarget->SetSurfaceID(0);	//Unmark this frames surface so it doesn't get deleted
	  while (lFrame) 
	    {
	      lFrame->DeleteSurfaceWithID(SurfaceID);
	      lFrame = lFrame->GetNextFrame();
	    }
	}
    }

  Progress * lProgress = new Progress();
//  if (!lProgress) 
  //  cout<<"Cannot allocate the pregress bar!"<<endl;

  if (AllFrames) 
    {	//compute the contour for each frame, no grid is kept
      long SurfaceID = mTarget->GetSurfaceID();
      long CurrentFrame = mData->GetCurrentFrame();
      long NumFrames = mData->GetNumFrames();

      for (int i=0; i<NumFrames; i++) 
	{
	  Orb3DSurface * lSurf;
	  lSurf = NULL;
	  mData->SetCurrentFrame(i+1);
	  Frame * lFrame = mData->GetCurrentFramePtr();

	  if (CurrentFrame != mData->GetCurrentFrame()) 
	    {
	      Surface * temp = lFrame->GetSurfaceWithID(SurfaceID);
	      //Confirm that the surface is the correct type

	      if (temp)
		if (temp->GetSurfaceType() == kOrb3DType)
		  lSurf = (Orb3DSurface *) temp;

	      if (lSurf) lSurf->UpdateData(mTarget);
	    } 
	  else lSurf = mTarget;

	  if (lSurf) 
	    {
	      if (Visible) 
		{
		  lProgress->ChangeText("Calculating 3D Grid�");
		  lProgress->SetBaseValue(100*i/NumFrames);
		  lProgress->SetScaleFactor((float) 0.9/NumFrames);
		  if (updateGrid) mTarget->CalculateMOGrid(mData, lProgress);
		  lProgress->ChangeText("Contouring grid�");
		  lProgress->SetBaseValue((long)(100*i/NumFrames + 90.0/NumFrames));
		  lProgress->SetScaleFactor((float) 0.1/NumFrames);
		  if (updateContour) mTarget->Contour3DGrid(lProgress);
		  lSurf->FreeGrid();
		} 
	      else 
		{
		  if (updateGrid) mTarget->FreeGrid();
		  if (updateContour) mTarget->FreeContour();
		}
	    }
	}
      mData->SetCurrentFrame(CurrentFrame);
    } 
  else 
    {	//simply update this surface
      if (Visible) 
	{
	  lProgress->ChangeText("Calculating 3D Grid�");
	  lProgress->SetScaleFactor(0.9);
	  if (updateGrid) mTarget->CalculateMOGrid(mData, lProgress);
	  lProgress->ChangeText("Contouring grid�");
	  lProgress->SetBaseValue(90);
	  lProgress->SetScaleFactor(0.1);
	  if (updateContour) mTarget->Contour3DGrid(lProgress);
	} 
      else 
	{
	  if (updateGrid) mTarget->FreeGrid();
	  if (updateContour) mTarget->FreeContour();
	}
    }
  if (lProgress) delete lProgress;

  if (mTarget->GridAvailable())
    mFreeMemBut->Enable();
  else
    mFreeMemBut->Disable();

  float GridMax = mTarget->GetGridMax();

  wxString tmpStr;
  tmpStr.Printf("%.4f", GridMax);
  mContourMaxTick->SetLabel(tmpStr);

  setContourValueSld();

//Setup the contour value and grid max text items

  UpdateTest = false;

  if (mTarget->ExportPossible())
    mExportBut->Enable();
  else
    mExportBut->Disable();

  mUpdateBut->Disable();

  owner->SurfaceUpdated();
}


bool Orbital3DSurfPane::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap Orbital3DSurfPane::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin Orbital3D bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end Orbital3D bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon Orbital3DSurfPane::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin Orbital3D icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end Orbital3D icon retrieval
}

Surface3DParamDlg::Surface3DParamDlg(BaseSurfacePane * parent, Surface * targetSurface, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
{
  mParent = parent;
  mTargetSurf = dynamic_cast<Surf3DBase*>(targetSurface);

  Create(id, caption, pos, size, style);
 
}
/*!
* General3DSurfPane class
 */

General3DSurfPane::General3DSurfPane( wxWindow* parent, General3DSurface* target, 
									  SurfacesWindow* o, wxWindowID id,
									  const wxPoint& pos, const wxSize& size, 
									  long style ) 
			: Surface3DPane(parent, target, o, id, pos, size, style)
{
	mTarget = target;
	
	TargetToPane();
	CreateControls();
}

General3DSurfPane::~General3DSurfPane()
{
	
}

void General3DSurfPane::TargetToPane(void) 
{
//	NumGridPoints = mTarget->GetNumGridPoints();
	mTarget->GetPosColor(&PosColor);
	mTarget->GetNegColor(&NegColor);
	mTarget->GetTranspColor(&TranspColor);
//	GridSize = mTarget->GetGridSize();
	ContourValue = mTarget->GetContourValue();
	UseSolidSurface = mTarget->SolidSurface();
	UseNormals = mTarget->UseSurfaceNormals();
	UpdateTest = false;
//	SwitchFixGrid = false;
}

void General3DSurfPane::refreshControls()
{
	float GridMax = mTarget->GetGridMax();
	
//	mNumGridPntSld->SetValue(NumGridPoints);
//	mGridSizeSld->SetValue((short)(100*GridSize));
	mContourValSld->SetValue((short)(100*(ContourValue/((fabs(GridMax)>=0.001)?GridMax:0.25))));
	m3DRdoBox->SetSelection(1-UseSolidSurface);
	mSmoothChkBox->SetValue(UseNormals);
	
	if (UseSolidSurface)
		mSmoothChkBox->Enable();
	else
		mSmoothChkBox->Disable();
	
//	mOrbColor1->draw(&PosColor);
//	mOrbColor2->draw(&NegColor);
//	mTransColor->draw(&TranspColor);
}

/*!
* Control creation General3D
 */

void General3DSurfPane::CreateControls()
{    
	float GridMax = mTarget->GetGridMax();
	
	upperSizer->Add(label0, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);

	upperLeftMiddleSizer->Layout();
	
	lowerLeftMiddleSizer->Add(label1, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);
	
	leftMiddleSizer->Add(upperLeftMiddleSizer, 0, wxALL, 3);
	leftMiddleSizer->Add(lowerLeftMiddleSizer, 0, wxALL, 3);
	
	rightMiddleSizer->Add(label3, 0, wxALIGN_CENTER_VERTICAL | wxALL, 10);
	rightMiddleSizer->Add(mContourValSld, 0, wxALIGN_CENTER_VERTICAL | wxALL, 10);

	wxString tmpStr;
	tmpStr.Printf("%.4f", ContourValue);
	m3DOrbMaxText->SetValue(tmpStr);
	middleSizer->Add(leftMiddleSizer, 0, wxALL, 10);
	middleSizer->Add(rightMiddleSizer, 0, wxALL, 10);
	bottomSizer->Add(leftBottomSizer, 0, wxALL, 3);
	bottomSizer->Add(rightBottomSizer, 0, wxALL, 3);
	mainSizer->Add(upperSizer);
	mainSizer->Add(middleSizer);
	mainSizer->Add(bottomSizer);
}

bool General3DSurfPane::UpdateNeeded(void) 
{
	bool result = false;
	
//	if (PlotOrb >= 0) 
    {	//Don't update unless a valid orbital is chosen
		if (Visible != mTarget->GetVisibility()) result = true;
		if (AllFrames != (mTarget->GetSurfaceID() != 0)) result = true;
		if (NumGridPoints != mTarget->GetNumGridPoints()) result = true;
		if (ContourValue != mTarget->GetContourValue()) result = true;
		if (GridSize != mTarget->GetGridSize()) result = true;
		if (UseSolidSurface != mTarget->SolidSurface()) result = true;
		if (UseNormals != mTarget->UseSurfaceNormals()) result = true;
//		if (PhaseChange != mTarget->GetPhaseChange()) result = true;
		
		if (!result) 
		{
			RGBColor	testColor;
			mTarget->GetPosColor(&testColor);
			if ((PosColor.red != testColor.red)||(PosColor.green!=testColor.green)
				||(PosColor.blue!=testColor.blue)) 
				result=true;
			
			mTarget->GetNegColor(&testColor);
			if ((NegColor.red != testColor.red)||(NegColor.green!=testColor.green)
				||(NegColor.blue!=testColor.blue)) 
				result=true;
			
			mTarget->GetTranspColor(&testColor);
			if ((TranspColor.red != testColor.red)
				||(TranspColor.green!=testColor.green)
				||(TranspColor.blue!=testColor.blue)) 
				result=true;
		}
    }
	return result;
}
bool General3DSurfPane::ShowToolTips()
{
    return true;
}

/*!
* Get bitmap resources
 */

wxBitmap General3DSurfPane::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
	////@begin Orbital3D bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
	////@end Orbital3D bitmap retrieval
}

/*!
* Get icon resources
 */

wxIcon General3DSurfPane::GetIconResource( const wxString& name )
{
    // Icon retrieval
	////@begin Orbital3D icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
	////@end Orbital3D icon retrieval
}

bool Surface3DParamDlg::Create(wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
{
  wxFrame::Create( mParent, id, caption, pos, size, style );

  createControls();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
  Centre();

  return true;
}

void Surface3DParamDlg::createControls()
{
  wxString tmpStr;
  CPoint3D tempPt;
  float tempFlt;

  mainSizer = new wxBoxSizer(wxVERTICAL);

  mainSizer->Add(new wxStaticText(this, wxID_ANY,
				  _T("Number of grid points (x, y, z):"),
				  wxDefaultPosition, wxDefaultSize), 
		 0, wxALIGN_LEFT | wxALL, 3);

  firstTierSizer = new wxBoxSizer(wxHORIZONTAL);
  numGridPoint1 = new wxTextCtrl(this, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize);
  tmpStr.Printf("%ld", mTargetSurf->GetNumXGridPoints());
  numGridPoint1->SetValue(tmpStr);

  numGridPoint2 = new wxTextCtrl(this, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize);
  tmpStr.Printf("%ld", mTargetSurf->GetNumYGridPoints());
  numGridPoint2->SetValue(tmpStr);

  numGridPoint3 = new wxTextCtrl(this, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize);
  tmpStr.Printf("%ld", mTargetSurf->GetNumZGridPoints());
  numGridPoint3->SetValue(tmpStr);

  firstTierSizer->Add(numGridPoint1, 0, wxALL, 5);
  firstTierSizer->Add(numGridPoint2, 0, wxALL, 5);
  firstTierSizer->Add(numGridPoint3, 0, wxALL, 5);

  mainSizer->Add(firstTierSizer);

  mainSizer->Add(new wxStaticText(this, wxID_ANY,
				  _T("Origin (x, y, z):"),
				  wxDefaultPosition, wxDefaultSize), 
		 0, wxALIGN_LEFT | wxALL, 3);

  secondTierSizer = new wxBoxSizer(wxHORIZONTAL);

  mTargetSurf->GetOrigin(&tempPt);

  originText1 = new wxTextCtrl(this, wxID_ANY, _T(""), wxDefaultPosition, wxSize(100, 25));
  tmpStr.Printf("%f", tempPt.x);
  originText1->SetValue(tmpStr);

  originText2 = new wxTextCtrl(this, wxID_ANY, _T(""), wxDefaultPosition, wxSize(100, 25));
  tmpStr.Printf("%f", tempPt.y);
  originText2->SetValue(tmpStr);

  originText3 = new wxTextCtrl(this, wxID_ANY, _T(""), wxDefaultPosition, wxSize(100, 25));
  tmpStr.Printf("%f", tempPt.z);
  originText3->SetValue(tmpStr);

  secondTierSizer->Add(originText1, 0, wxALL, 5);
  secondTierSizer->Add(originText2, 0, wxALL, 5);
  secondTierSizer->Add(originText3, 0, wxALL, 5);

  mainSizer->Add(secondTierSizer);

  mainSizer->Add(new wxStaticText(this, wxID_ANY,
				  _T("Grid increment (x, y, z):"),
				  wxDefaultPosition, wxDefaultSize), 
		 0, wxALIGN_LEFT | wxALL, 3);

  thirdTierSizer = new wxBoxSizer(wxHORIZONTAL);
  gridIncText1 = new wxTextCtrl(this, wxID_ANY, _T(""), wxDefaultPosition, wxSize(100, 25));
  tmpStr.Printf("%f", mTargetSurf->GetXGridInc());
  gridIncText1->SetValue(tmpStr);

  gridIncText2 = new wxTextCtrl(this, wxID_ANY, _T(""), wxDefaultPosition, wxSize(100, 25));
  tmpStr.Printf("%f", mTargetSurf->GetYGridInc());
  gridIncText2->SetValue(tmpStr);

  gridIncText3 = new wxTextCtrl(this, wxID_ANY, _T(""), wxDefaultPosition, wxSize(100, 25));
  tmpStr.Printf("%f", mTargetSurf->GetZGridInc());
  gridIncText3->SetValue(tmpStr);

  thirdTierSizer->Add(gridIncText1, 0, wxALL, 5);
  thirdTierSizer->Add(gridIncText2, 0, wxALL, 5);
  thirdTierSizer->Add(gridIncText3, 0, wxALL, 5);

  mainSizer->Add(thirdTierSizer);

  fourthTierSizer = new wxBoxSizer(wxHORIZONTAL);
  okButton = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize);
  cancelButton = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize);
  copyAllButton = new wxButton( this, ID_COPY_ALL, _("&Copy All"), wxDefaultPosition, wxDefaultSize);
  pasteAllButton = new wxButton( this, ID_PASTE_ALL, _("&Paste All"), wxDefaultPosition, wxDefaultSize);

  fourthTierSizer->Add(copyAllButton , 0, wxALL, 5);
  fourthTierSizer->Add(pasteAllButton , 0, wxALL, 5);
  fourthTierSizer->Add(cancelButton , 0, wxALL, 5);
  fourthTierSizer->Add(okButton , 0, wxALL, 5);

  mainSizer->Add(fourthTierSizer);

  SetSizer(mainSizer);
}

void Surface3DParamDlg::OnClose(wxCommandEvent &event )
{
  wxString tmpStr;
  long tempLong;
  CPoint3D tempPt;
  float	tempFlt;

  tmpStr = numGridPoint1->GetValue();
  tempLong = atol(tmpStr.c_str());
  mTargetSurf->SetNumXGridPoints(tempLong);

  tmpStr = numGridPoint2->GetValue();
  tempLong = atol(tmpStr.c_str());		
  mTargetSurf->SetNumYGridPoints(tempLong);
				
  tmpStr = numGridPoint3->GetValue();
  tempLong = atol(tmpStr.c_str());
  mTargetSurf->SetNumZGridPoints(tempLong);

  tmpStr = originText1->GetValue();
  tempPt.x = atof(tmpStr.c_str());
  tmpStr = originText2->GetValue();
  tempPt.y = atof(tmpStr.c_str());
  tmpStr = originText3->GetValue();
  tempPt.z = atof(tmpStr.c_str());

  mTargetSurf->SetOrigin(&tempPt);

  tmpStr = gridIncText1->GetValue();
  tempFlt = atof(tmpStr);
  mTargetSurf->SetXGridInc(tempFlt);

  tmpStr = gridIncText2->GetValue();
  tempFlt = atof(tmpStr);
  mTargetSurf->SetYGridInc(tempFlt);			
			       
  tmpStr = gridIncText3->GetValue();
  tempFlt = atof(tmpStr);
  mTargetSurf->SetZGridInc(tempFlt);
	
  mTargetSurf->SetFixGrid(true);

  mParent->TargetToPane();
  mParent->refreshControls();
  mParent->SetUpdateTest(true);

  Destroy();
}

void Surface3DParamDlg::OnCancel(wxCommandEvent &event )
{
  Destroy();
}

/*!!! Use wxWidgets' config class to implement copyAll and pasteAll
  instead of operating on a file directly
*/

void Surface3DParamDlg::OnCopyAll(wxCommandEvent &event )
{
  wxConfigBase *pConfig = wxConfigBase::Get();

  pConfig->Write(_T("/Parameters/Surface3D/NumGridPointsX"), numGridPoint1->GetValue());
  pConfig->Write(_T("/Parameters/Surface3D/NumGridPointsY"), numGridPoint2->GetValue());
  pConfig->Write(_T("/Parameters/Surface3D/NumGridPointsZ"), numGridPoint3->GetValue());

  pConfig->Write(_T("/Parameters/Surface3D/OriginX"), originText1->GetValue());
  pConfig->Write(_T("/Parameters/Surface3D/OriginY"), originText2->GetValue());
  pConfig->Write(_T("/Parameters/Surface3D/OriginZ"), originText3->GetValue());

  pConfig->Write(_T("/Parameters/Surface3D/XInc"), gridIncText1->GetValue());
  pConfig->Write(_T("/Parameters/Surface3D/YInc"), gridIncText2->GetValue());
  pConfig->Write(_T("/Parameters/Surface3D/ZInc"), gridIncText3->GetValue());

}

void Surface3DParamDlg::OnPasteAll(wxCommandEvent &event )
{
  wxConfigBase *pConfig = wxConfigBase::Get();

  pConfig->SetPath(_T("/Parameters/Surface3D"));

  numGridPoint1->SetValue(pConfig->Read(_T("NumGridPointsX")));
  numGridPoint2->SetValue(pConfig->Read(_T("NumGridPointsY")));
  numGridPoint3->SetValue(pConfig->Read(_T("NumGridPointsZ")));

  originText1->SetValue(pConfig->Read(_T("OriginX")));
  originText2->SetValue(pConfig->Read(_T("OriginY")));
  originText3->SetValue(pConfig->Read(_T("OriginZ")));

  gridIncText1->SetValue(pConfig->Read(_T("XInc")));
  gridIncText2->SetValue(pConfig->Read(_T("YInc")));
  gridIncText3->SetValue(pConfig->Read(_T("ZInc")));
}
