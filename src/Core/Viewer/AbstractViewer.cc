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

// Core includes
#include <Core/Viewer/AbstractViewer.h>


namespace Core
{

class AbstractViewerPrivate
{
public:
	// Handle with the renderer
	AbstractRendererHandle renderer_;
	
	// Width and height of the viewer window
	int width_;
	int height_;

	// Last rendered Texture and Overlay Texture generated by the renderer
	Texture2DHandle texture_;
	Texture2DHandle overlay_texture_;

	AbstractViewer* viewer_;

	// SET_TEXTURE:
	// Connected to the redraw_completed_signal_ of the renderer.
	void set_texture( Texture2DHandle texture, bool delay_update );

	// SET_OVERLAY_TEXTURE:
	// Connected to the redraw_overlay_completed_signal_ of the renderer.
	void set_overlay_texture( Texture2DHandle texture, bool delay_update );

};

void AbstractViewerPrivate::set_texture( Texture2DHandle texture, bool delay_update )
{
	{
		AbstractViewer::lock_type lock( this->viewer_->get_mutex() );
		this->texture_ = texture;
	}

	if ( !delay_update )
	{
		this->viewer_->update_display_signal_();
	}
}

void AbstractViewerPrivate::set_overlay_texture( Texture2DHandle texture, bool delay_update )
{
	{
		AbstractViewer::lock_type lock( this->viewer_->get_mutex() );
		this->overlay_texture_ = texture;
	}

	if ( !delay_update )
	{
		this->viewer_->update_display_signal_();
	}
}

AbstractViewer::AbstractViewer( size_t viewer_id, size_t version_number ) :
	StateHandler( std::string( "viewer" ) + Core::ExportToString( viewer_id ), version_number, false ),
	private_( new AbstractViewerPrivate )
{
	// ensure these are initialized
	this->private_->width_ = 0;
	this->private_->height_ = 0;
	this->private_->viewer_ = this;
	
	add_state( "viewer_visible", this->viewer_visible_state_, false );
}

AbstractViewer::~AbstractViewer()
{
	this->disconnect_all();
}

size_t AbstractViewer::get_viewer_id() const
{
	return this->get_statehandler_id_number();
}

void AbstractViewer::resize( int width, int height )
{
	this->private_->width_ = width;
	this->private_->height_ = height;
	this->private_->renderer_->resize( width, height );
}

void AbstractViewer::mouse_move_event( const MouseHistory& mouse_history, 
									  int button, int buttons, int modifiers )
{
	// Do nothing
}

void AbstractViewer::mouse_press_event( const MouseHistory& mouse_history, 
									   int button, int buttons, int modifiers )
{
	// Do nothing
}

void AbstractViewer::mouse_release_event( const MouseHistory& mouse_history, 
										 int button, int buttons, int modifiers )
{
	// Do nothing
}

bool AbstractViewer::wheel_event( int delta, int x, int y, int buttons, int modifiers )
{
	// Do nothing
	return false;
}

bool AbstractViewer::key_event( int key, int modifiers )
{
	// do nothing
	return false;
}

void AbstractViewer::install_renderer( AbstractRendererHandle renderer )
{
	{
		lock_type lock( this->get_mutex() );

		if ( this->private_->renderer_ )
		{
			CORE_THROW_LOGICERROR( "Cannot install renderer twice into viewer" );
		}
		
		this->private_->renderer_ = renderer;
	}
	
	this->add_connection( this->private_->renderer_->redraw_completed_signal_.connect(
		boost::bind( &AbstractViewerPrivate::set_texture, this->private_, _1, _2 ) ) );
	
	this->add_connection( this->private_->renderer_->redraw_overlay_completed_signal_.connect(
		boost::bind( &AbstractViewerPrivate::set_overlay_texture, this->private_, _1, _2 ) ) );
}

AbstractRendererHandle AbstractViewer::get_renderer()
{
	lock_type lock( this->get_mutex() );
	return this->private_->renderer_;
}

Texture2DHandle AbstractViewer::get_texture()
{
	lock_type lock( this->get_mutex() );
	return this->private_->texture_;
}

Texture2DHandle AbstractViewer::get_overlay_texture()
{
	lock_type lock( this->get_mutex() );
	return this->private_->overlay_texture_;
}

int AbstractViewer::get_width()
{
	lock_type lock( this->get_mutex() );
	return this->private_->width_;
}

int AbstractViewer::get_height()
{
	lock_type lock( this->get_mutex() );
	return this->private_->height_;
}

} // end namespace Core

