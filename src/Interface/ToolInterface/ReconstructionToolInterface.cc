/*
 For more information, please see: http://software.sci.utah.edu
 
 The MIT License
 
 Copyright (c) 2013 Scientific Computing and Imaging Institute,
 University of Utah.
 
 
 Permission is hereby granted, free of charge, to any person obtaining a
 copy of this software and associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.
 */


// Boost includes
#include <boost/filesystem.hpp>

// Core includes
#include <Core/Interface/Interface.h>
#include <Core/Utils/Log.h>

// Qt includes
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtCore/QString>

// Qt Gui Includes
#include <QtUtils/Bridge/QtBridge.h>

// Core Includes
#include <Core/Utils/Log.h>
#include <Core/State/Actions/ActionSet.h>

// Interface Includes
#include <Interface/ToolInterface/ReconstructionToolInterface.h>
#include <Interface/Application/LayerImporterWidget.h>

#include "ui_ReconstructionToolInterface.h"

// Application Includes
#include <Application/Layer/LayerManager.h>
#include <Application/LayerIO/LayerIO.h>
//#include <Application/ViewerManager/Actions/ActionPickPoint.h>
#include <Application/PreferencesManager/PreferencesManager.h>
#include <Application/ProjectManager/ProjectManager.h>

#include <Application/Layer/Actions/ActionActivateLayer.h>

#include <Application/BackscatterReconstruction/ReconstructionTool.h>

// Core Includes
#include <Core/State/Actions/ActionSetAt.h>

// test
#include <iostream>
// test

SCI_REGISTER_TOOLINTERFACE( Seg3D, ReconstructionToolInterface )

namespace bfs=boost::filesystem;

namespace Seg3D
{

class ReconstructionToolInterfacePrivate
{
public:
  Ui::ReconstructionToolInterface ui_;
  ReconstructionToolInterface* interface_;  

  void importLabelNrrd();
  void setOutputDirectory();
};

void ReconstructionToolInterfacePrivate::setOutputDirectory()
{
  // propagate to tool state
  QDir outputDir = QDir( QFileDialog::getExistingDirectory ( interface_, 
    interface_->tr( "Choose Output Directory" ), this->ui_.outputDirLineEdit->text(), 
    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks ) );
  if( outputDir.exists() )
  {
    this->ui_.outputDirLineEdit->setText( outputDir.canonicalPath() );
  }
  else
  {
    this->ui_.outputDirLineEdit->setText( "" );
  }
}


void ReconstructionToolInterfacePrivate::importLabelNrrd()
{
  bfs::path current_file_folder = ProjectManager::Instance()->get_current_file_folder();
  
  // Bring up the file dialog
  QString filtername;
  QStringList file_list;
  
  // Get the importer list from the LayerIO system
  std::vector< std::string > importer_types = LayerIO::Instance()->get_single_file_importer_types();
  
  QString filters;
  for ( size_t j = 0; j < importer_types.size(); j++ )
  {
    // TODO: .nrrd only
    if ( j == 0 )
    {
      filters = QString::fromStdString( importer_types[ j ] );
      continue;
    }
    filters = filters + ";;" + QString::fromStdString( importer_types[ j ] );
  }
  
  // Use native dialog
  // TODO: not likely to be needed on Linux, but if so, test this!!!
  file_list = QFileDialog::getOpenFileNames( 0, "Select labels file ",
                                            current_file_folder.string().c_str(), filters, &filtername );
  
  // TODO: basically copied from LayerIOFunctions...
  // Make common utility function or just use .his and ITK .tif importers...
  if( file_list.size() == 0 )
  {
    // log no files selected
    return;
  }
  
  std::string importer_name = filtername.toStdString();
  
  LayerImporterHandle importer;
  // TODO: specialize to just masks?
  if( ! ( LayerIO::Instance()->create_single_file_importer( file_list.at( 0 ).toStdString(), importer, 
                                                           importer_name ) ) )
  {
    // If we are unable to create an importer we pop up an error message box
    std::string error_message = std::string( "ERROR: No importer is available for file '" ) + 
    file_list.at( 0 ).toStdString() + std::string( "'." );
    
    QMessageBox message_box;
    message_box.setWindowTitle( "Import Layer Error" );
    message_box.addButton( QMessageBox::Ok );
    message_box.setIcon( QMessageBox::Critical );
    message_box.setText( QString::fromStdString( error_message ) );
    message_box.exec();
    QString fileStem( bfs::path( file_list.at( 0 ).toStdString() ).stem().string().c_str() );
    //ui_.fileNameLabel->setText( fileStem );
    return;
  }
  else
  {
    
    // List of importers to use
    std::vector< LayerImporterHandle > importers;
    
    // Loop over all the selected files and generate an importer for them
    for( int i = 0; i < file_list.size(); ++i )
    {
      LayerImporterHandle importer;
      std::string error;
      
      // Create a new importer
      if( ! ( LayerIO::Instance()->create_single_file_importer( file_list.at( i ).toStdString(), 
                                                               importer, error, importer_name ) ) )
      {
        // Failed to create the importer, and warn the user explicitly
        std::string error_message = std::string( "ERROR: No importer is available for file '" ) 
        + file_list.at( i ).toStdString() + std::string( "'. " ) + error;
        
        QMessageBox message_box;
        message_box.setWindowTitle( "Import Layer Error" );
        message_box.addButton( QMessageBox::Ok );
        message_box.setIcon( QMessageBox::Critical );
        message_box.setText( QString::fromStdString( error_message ) );
        
        // Send message box to Qt
        message_box.exec();
        return;
      }
      else
      {
        // Add the importer to the list of all the importers that need to be handled
        importers.push_back( importer );
      }
    }
    
    // Open the importer dialog that issues the action to import the data file(s)
    LayerImporterWidget layer_import_dialog( importers, 0 );
    layer_import_dialog.exec();
  }
}
  

// constructor
ReconstructionToolInterface::ReconstructionToolInterface() :
private_( new ReconstructionToolInterfacePrivate )
{
    this->private_->interface_ = this;
}

// destructor
ReconstructionToolInterface::~ReconstructionToolInterface()
{
  this->disconnect_all();
}


void ReconstructionToolInterface::triggerSetOutputDir()
{
  this->private_->setOutputDirectory();
}


void ReconstructionToolInterface::triggerLabelImport()
{
  this->private_->importLabelNrrd();
}

void ReconstructionToolInterface::update_progress_bar( double progress )
{
  this->private_->ui_.progress_bar_->setValue(progress);
}

void ReconstructionToolInterface::UpdateProgress( qpointer_type qpointer, double progress )
{
  // Hand it off to the right thread
  if( !( Core::Interface::IsInterfaceThread() ) )
  {
    Core::Interface::Instance()->post_event(
      boost::bind( &ReconstructionToolInterface::UpdateProgress, qpointer, progress) );
    return;	
  }
  
  // When we are finally on the interface thread run this code:
  if ( qpointer.data() && !QCoreApplication::closingDown() )
  {
    qpointer->update_progress_bar( progress );
//    qpointer->parent_->update();
  }
}

bool ReconstructionToolInterface::build_widget( QFrame* frame )
{
  this->private_->ui_.setupUi( frame );
  
  // TODO: make tool and action for this for scripting (take filepath, filter)
  connect( this->private_->ui_.setDirButton, SIGNAL( clicked() ), this, SLOT( triggerSetOutputDir() ) );

  // TODO: make tool and action for this for scripting (take filepath, filter)
  //connect( this->private_->ui_.openDataButton, SIGNAL( clicked() ), this, SLOT( triggerLabelImport() ) );
  connect( this->private_->ui_.openLabelsButton, SIGNAL( clicked() ), this, SLOT( triggerLabelImport() ) );
 
	ToolHandle base_tool_ = tool();
	ReconstructionTool* tool = dynamic_cast< ReconstructionTool* > ( base_tool_.get() );

	QtUtils::QtBridge::Connect( this->private_->ui_.iterationsCombo, tool->iterations_state_ );
	QtUtils::QtBridge::Connect( this->private_->ui_.outputDirLineEdit, tool->outputDirectory_state_, true );
	QtUtils::QtBridge::Connect( this->private_->ui_.runFilterButton, boost::bind(
    &Tool::execute, tool, Core::Interface::GetWidgetActionContext() ) );
  
  this->add_connection( tool->update_progress_signal_.connect(
    boost::bind( &ReconstructionToolInterface::UpdateProgress, qpointer_type( this ), _1 ) ) );

	this->private_->ui_.iterationsCombo->set_description( "Iterations" );
  
  return true;
}
  
} // end namespace Seg3D