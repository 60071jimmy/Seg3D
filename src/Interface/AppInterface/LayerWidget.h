/*
 For more information, please see: http://software.sci.utah.edu

 The MIT License

 Copyright (c) 2009 Scientific Computing and Imaging Institute,
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

#ifndef INTERFACE_APPINTERFACE_LAYERWIDGET_H
#define INTERFACE_APPINTERFACE_LAYERWIDGET_H

// QT Includes
#include <QtGui>

// Application includes
#include <Application/Layer/Layer.h>


namespace Seg3D
{
  
class LayerGroupWidget;
typedef QSharedPointer< LayerGroupWidget > LayerGroupWidgetQHandle;
  
class LayerWidget;
typedef QSharedPointer< LayerWidget > LayerWidgetQHandle;
typedef QWeakPointer< LayerWidget > LayerWidgetQWeakHandle;


class LayerWidgetPrivate;
  
class LayerWidget : public QWidget
{
Q_OBJECT

// -- constructor/destructor --
public:
  LayerWidget( QFrame* parent, LayerHandle layer );
  virtual ~LayerWidget();
    
// -- widget internals --
public Q_SLOTS:
  void show_opacity_bar( bool show );
  void show_brightness_contrast_bar( bool show );
  void show_border_fill_bar( bool show );
  void show_color_bar( bool show );
  void show_progress_bar( bool show );
  void visual_lock( bool lock );
  
public:
  void show_selection_checkbox( bool hideshow );
  void set_active( bool active );
  void set_drop( bool drop );
  void seethrough( bool see );
  void set_group_menu_status( bool status );

  
private:
    boost::shared_ptr< LayerWidgetPrivate > private_;

private:
  // icons to represent the layer types
  QIcon data_layer_icon_;
  QIcon label_layer_icon_;
  QIcon mask_layer_icon_;
  
  std::string layer_id_;
  Utils::GridTransform grid_transform_;
  int volume_type_;
  bool active_;
  bool picked_up_;
  bool group_menus_open_;
  LayerWidget* drop_layer_;

  
  void set_drop_target( LayerWidget* target_layer );
  void mousePressEvent( QMouseEvent* event );
//  void dragMoveEvent( QDragMoveEvent* event );
  void dropEvent( QDropEvent* event );
  void dragEnterEvent( QDragEnterEvent* event );
  void dragLeaveEvent( QDragLeaveEvent* event );

//private Q_SLOTS:
//  void shrink();
//  void grow();

protected:
  void resizeEvent( QResizeEvent *event );

public:
  int get_volume_type(){ return this->volume_type_; }
  std::string& get_layer_id(){ return this->layer_id_; }
  void set_picked_up( bool up ){ this->picked_up_ = up; }
  


};

} //end namespace Seg3D

#endif
