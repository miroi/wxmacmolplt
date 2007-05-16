/////////////////////////////////////////////////////////////////////////////
// Name:        aboutwxmacmolplt.cpp
// Purpose:     
// Author:      Brett Bode
// Created:     Tue 23 May 13:01:58 2006
// Copyright:   (c) 2006 Iowa State University
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// Generated by DialogBlocks (unregistered), Tue 23 May 13:01:58 2006

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "aboutwxmacmolplt.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes
#include "Globals.h"
#include "aboutwxmacmolplt.h"

////@begin XPM images
////@end XPM images

/*!
 * AboutwxMacMolPlt type definition
 */

IMPLEMENT_DYNAMIC_CLASS( AboutwxMacMolPlt, wxDialog )

/*!
 * AboutwxMacMolPlt event table definition
 */

BEGIN_EVENT_TABLE( AboutwxMacMolPlt, wxDialog )

////@begin AboutwxMacMolPlt event table entries
////@end AboutwxMacMolPlt event table entries

END_EVENT_TABLE()

/*!
 * AboutwxMacMolPlt constructors
 */

AboutwxMacMolPlt::AboutwxMacMolPlt( )
{
}

AboutwxMacMolPlt::AboutwxMacMolPlt( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * AboutwxMacMolPlt creator
 */

bool AboutwxMacMolPlt::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin AboutwxMacMolPlt member initialisation
	mVersionText = NULL;
////@end AboutwxMacMolPlt member initialisation

////@begin AboutwxMacMolPlt creation
	SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
	wxDialog::Create( parent, id, caption, pos, size, style );

	CreateControls();
	if (GetSizer())
	{
		GetSizer()->SetSizeHints(this);
	}
	Centre();
////@end AboutwxMacMolPlt creation
    return true;
}

/*!
 * Control creation for AboutwxMacMolPlt
 */

void AboutwxMacMolPlt::CreateControls()
{    
////@begin AboutwxMacMolPlt content construction
	AboutwxMacMolPlt* itemDialog1 = this;

	wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
	itemDialog1->SetSizer(itemBoxSizer2);

	mVersionText = new wxStaticText( itemDialog1, wxID_STATIC, _("MacMolPlt v6.0\nBy Brett Bode"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	itemBoxSizer2->Add(mVersionText, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

	wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
	itemBoxSizer2->Add(itemBoxSizer4, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

	wxStaticText* itemStaticText5 = new wxStaticText( itemDialog1, wxID_STATIC, _("FreeWare--Please let me\nknow if you use MacMolPlt. If you\nwish to see further future\ndevelopment then let me know\nthat you use my program!"), wxDefaultPosition, wxDefaultSize, 0 );
	itemBoxSizer4->Add(itemStaticText5, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

	wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxVERTICAL);
	itemBoxSizer4->Add(itemBoxSizer6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	wxStaticText* itemStaticText7 = new wxStaticText( itemDialog1, wxID_STATIC, _("Available via the WWW URL:"), wxDefaultPosition, wxDefaultSize, 0 );
	itemBoxSizer6->Add(itemStaticText7, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

#if wxCHECK_VERSION(2, 8, 0)
	wxHyperlinkCtrl* itemHyperlinkCtrl8 = new wxHyperlinkCtrl( itemDialog1, ID_HYPERLINKCTRL2, _("http://www.scl.ameslab.gov/MacMolPlt/"), _T("http://www.scl.ameslab.gov/MacMolPlt/"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
#else
	wxStaticText* itemHyperlinkCtrl8 = new wxStaticText( itemDialog1, ID_HYPERLINKCTRL2, _("http://www.scl.ameslab.gov/MacMolPlt/"), wxDefaultPosition, wxDefaultSize);
#endif
	itemBoxSizer6->Add(itemHyperlinkCtrl8, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

	wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxVERTICAL);
	itemBoxSizer2->Add(itemBoxSizer9, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

	wxBoxSizer* itemBoxSizer10 = new wxBoxSizer(wxHORIZONTAL);
	itemBoxSizer9->Add(itemBoxSizer10, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

	wxStaticText* itemStaticText11 = new wxStaticText( itemDialog1, wxID_STATIC, _("Please take the:"), wxDefaultPosition, wxDefaultSize, 0 );
	if (ShowToolTips())
		itemStaticText11->SetToolTip(_("Just send an email"));
	itemBoxSizer10->Add(itemStaticText11, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

#if wxCHECK_VERSION(2, 8, 0)
	wxHyperlinkCtrl* itemHyperlinkCtrl12 = new wxHyperlinkCtrl( itemDialog1, ID_HYPERLINKCTRL1, _("MacMolPlt survey"), _T("http://www.scl.ameslab.gov/MacMolPlt/MacMolPltSurvey.html"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
#else
	wxStaticText* itemHyperlinkCtrl12 = new wxStaticText( itemDialog1, ID_HYPERLINKCTRL1, _("http://www.scl.ameslab.gov/MacMolPlt/MacMolPltSurvey.html"), wxDefaultPosition, wxDefaultSize);
#endif
	itemBoxSizer10->Add(itemHyperlinkCtrl12, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	wxBoxSizer* itemBoxSizer13 = new wxBoxSizer(wxHORIZONTAL);
	itemBoxSizer9->Add(itemBoxSizer13, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

	wxStaticText* itemStaticText14 = new wxStaticText( itemDialog1, wxID_STATIC, _("or send an email to "), wxDefaultPosition, wxDefaultSize, 0 );
	itemBoxSizer13->Add(itemStaticText14, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

#if wxCHECK_VERSION(2, 8, 0)
	wxHyperlinkCtrl* itemHyperlinkCtrl15 = new wxHyperlinkCtrl( itemDialog1, ID_HYPERLINKCTRL3, _("Brett."), _T("mailto:brett@scl.ameslab.gov"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
#else
	wxStaticText* itemHyperlinkCtrl15 = new wxStaticText( itemDialog1, ID_HYPERLINKCTRL3, _("brett@scl.ameslab.gov"), wxDefaultPosition, wxDefaultSize);
#endif
	itemBoxSizer13->Add(itemHyperlinkCtrl15, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	wxStaticText* itemStaticText16 = new wxStaticText( itemDialog1, wxID_STATIC, _("Please reference: Bode, B. M. and Gordon, M. S. J. Mol. Graphics and\nModeling, 1998, 16, 133-138."), wxDefaultPosition, wxDefaultSize, 0 );
	itemBoxSizer2->Add(itemStaticText16, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

	wxStaticText* itemStaticText17 = new wxStaticText( itemDialog1, wxID_STATIC, _("The primary developer of MacMolPlt is Brett Bode with assistance over the past couple of years from several talented students including Chris Johnson, Song Li, and Steven Schulteis.\n\nThanks to the Gordon Group at Iowa State University, Apple Computer and the National Science Foundation for providing programming support."), wxDefaultPosition, wxDefaultSize, 0 );
	itemStaticText17->Wrap(300);
	if (ShowToolTips())
		itemStaticText17->SetToolTip(_("Thanks even to Kurt..."));
	itemBoxSizer2->Add(itemStaticText17, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

	wxButton* itemButton18 = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	itemBoxSizer2->Add(itemButton18, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

////@end AboutwxMacMolPlt content construction
    mVersionText->SetLabel(_("MacMolPlt v"wxMacMolPlt_VERSION"\nBy Brett Bode"));
}

/*!
 * Should we show tooltips?
 */

bool AboutwxMacMolPlt::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap AboutwxMacMolPlt::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin AboutwxMacMolPlt bitmap retrieval
	wxUnusedVar(name);
	return wxNullBitmap;
////@end AboutwxMacMolPlt bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon AboutwxMacMolPlt::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin AboutwxMacMolPlt icon retrieval
	wxUnusedVar(name);
	return wxNullIcon;
////@end AboutwxMacMolPlt icon retrieval
}
