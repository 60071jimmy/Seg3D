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

#ifndef APPLICATION_LAYERMANAGER_LAYERMANAGER_H
#define APPLICATION_LAYERMANAGER_LAYERMANAGER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif 

// Boost includes
#include <boost/shared_ptr.hpp>

// Core includes
#include <Core/State/StateHandler.h>
#include <Core/Utils/Singleton.h>
#include <Core/Volume/Volume.h>
#include <Core/Geometry/BBox.h>

// Application includes
#include <Application/Layer/Layer.h>
#include <Application/Layer/DataLayer.h>
#include <Application/Layer/MaskLayer.h>
#include <Application/Project/Project.h>

namespace Seg3D
{

// Forward declaration
class LayerManager;
class LayerManagerPrivate;
typedef boost::shared_ptr< LayerManagerPrivate > LayerManagerPrivateHandle;

// Class definition
class LayerManager : public Core::StateHandler
{
	CORE_SINGLETON( LayerManager );

	// -- Constructor/Destructor --
private:
	LayerManager();
	virtual ~LayerManager();
	
	// -- Set up StateHandler priority --
public:
	// TODO: Fix this and put this in a macro definition
	// --JS
	virtual int get_session_priority() { return SessionPriority::LAYER_MANAGER_PRIORITY_E; }

	// -- Accessor Functions --
public:
    // Functions for getting a copy of the Layers and Groups with the proper locking
	// GET_GROUPS:
	// this function copies the groups into the vector that is passed
	void get_groups( std::vector< LayerGroupHandle >& vector_of_groups );
	
	// GET_LAYERS:
	// this function copies the layers into the vector that is passed
	void get_layers( std::vector< LayerHandle >& vector_of_layers );
	
	void get_layers_in_order( std::vector< LayerHandle >& vector_of_layers );
	
	// GET_LAYERS_IN_GROUP:
	// this function copies the layers into the vector that is passed
	void get_layers_in_group( LayerGroupHandle group, std::vector< LayerHandle > &vector_of_layers);
	
	// CHECK_FOR_SAME_GROUP:
	// this function returns true if the layer id's that are passed are from layer's in the same
	// group
	bool check_for_same_group( const std::string layer_to_insert_id, 
		const std::string layer_below_id );
	
	// GET_GROUP_BY_ID:
	// this function returns the group with the id that is passed
	LayerGroupHandle get_group_by_id( std::string group_id );

	// GET_GROUP_BY_PROVENANCE_ID:
	// this function returns the group with the id that is passed
	LayerGroupHandle get_group_by_provenance_id( ProvenanceID provenance_id );
	
	// GET_LAYER_BY_ID:
	// this function returns a handle to the layer with the id that is passed
	LayerHandle get_layer_by_id( const std::string& layer_id );
	
	// GET_LAYER_BY_NAME:
	// this function returns a handle to a layer with the name that is passed
	LayerHandle get_layer_by_name( const std::string& layer_name );

	// GET_LAYER_BY_PROVENANCE_ID:
	// this function returns a handle to a layer with the provenance id that was given
	LayerHandle get_layer_by_provenance_id( ProvenanceID provenance_id );

	// GET_DATA_LAYER_BY_ID:
	// this function returns a handle to the layer with the id that is passed
	DataLayerHandle get_data_layer_by_id( const std::string& layer_id );

	// GET_MASK_LAYER_BY_ID:
	// this function returns a handle to the layer with the id that is passed
	MaskLayerHandle get_mask_layer_by_id( const std::string& layer_id );

	// GET_ACTIVE_LAYER:
	// This function returns a handle to the active layer
	// Locks: StateEngine
	LayerHandle get_active_layer();
	
	// GET_LAYER_NAMES:
	// This function returns a vector of layer ID and name pairs of the specified layer type.
	void get_layer_names( std::vector< LayerIDNamePair >& layer_names, 
		int type = Core::VolumeType::ALL_E );

	// GET_LAYER_NAMES:
	// This function returns a vector of layer ID and name pairs of the specified layer type.
	void get_layer_names_from_group( LayerGroupHandle group,
		std::vector< LayerIDNamePair >& layer_names, 
		int type = Core::VolumeType::ALL_E );

	// GET_GROUP_POSITION:
	// Returns the position of the given group.
	size_t get_group_position( LayerGroupHandle group );

	// Layer Action Functions
public:
	// INSERT_LAYER:
	// This function returns true when it successfully inserts a layer
	bool insert_layer( LayerHandle layer );
	
	// MOVE_LAYER_ABOVE:
	// this function returns true when it has successfully inserted a layer above another layer
	bool move_layer_above( LayerHandle layer_to_move, LayerHandle target_layer );

	// MOVE_LAYER_BELOW:
	// this function returns true when it has successfully inserted a layer above another layer
	bool move_layer_below( const std::string& layer_id, const std::string& group_id );

	// DELETE_LAYERS:
	// this function deletes the selected layers in the group that is passed
	void delete_layers( std::vector< LayerHandle > layers );

	// SET_ACTIVE_LAYER:
	// this function sets the active layer
	void set_active_layer( LayerHandle layer );
	
	// SET_NEXT_LAYER_ACTIVE:
	// this function sets the active layer
	void set_next_layer_active();
	
	// SET_PREVIOUS_LAYER_ACTIVE:
	// this function sets the active layer
	void set_previous_layer_active();

	// UNDELETE_LAYERS:
	// Add the deleted layers back.
	// The first parameter contains a vector of the layers, the second parameter contains
	// the original position of the group that contained the layer, the third parameter
	// contains the original position of the layer within its group.
	// NOTE: The three parameters must have the same number of elements.
	void undelete_layers( const std::vector< LayerHandle >& layers, 
		const std::vector< size_t >& group_pos, const std::vector< size_t >& layer_pos );

private:
	// VALIDATE_LAYER_MOVE:
	// this function is used to validate that a layer move is the kind we want to allow. 
	// Currently the layers have to be the same type to successfully move.
	bool validate_layer_move( LayerHandle layer_above, LayerHandle layer_below );

public:
	// Take an atomic snapshot of visual properties of layers for rendering in the specified viewer
	LayerSceneHandle compose_layer_scene( size_t viewer_id );

	// Get the bounding box of all layers
	Core::BBox get_layers_bbox();
	
	// Group Action Functions
public:
	// MOVE_GROUP_ABOVE:
	// this function handle changing the order of a group in the list
	bool move_group_above( std::string group_to_move_id, std::string group_below_id );

private:
	// INSERT_GROUP:
	// this function handle inserting a group into the list
	void insert_group( LayerGroupHandle group_above, LayerGroupHandle group_below );

	// HANDLE_LAYER_NAME_CHANGED:
	// This function is connected to the value_changed_signal_ of all the layers and relays
	// it to the layer_name_changed_signal_ of LayerManager.
	void handle_layer_name_changed( std::string layer_id, std::string name );
	
	// HANDLE_LAYER_DATA_CHANGED:
	// This function keeps track off when layer data is changed.
	void handle_layer_data_changed( LayerHandle layer );

	// FIND_FREE_COLOR:
	// Find a color that has not yet been used.
	int find_free_color();

	// -- locking --
public: 
	typedef Core::StateEngine::mutex_type mutex_type;
	typedef Core::StateEngine::lock_type lock_type;
	
	// GET_MUTEX:
	// Get the mutex, so it can be locked by the interface that is built
	// on top of this
	mutex_type& get_mutex();
	
	// state variables
public:
	Core::StateLabeledOptionHandle active_layer_state_;

public:
	// TODO: There are too many signals in here, we should clean this up
	// --JGS

	// -- Signal/Slots --
	typedef boost::signals2::signal< void( LayerHandle ) > layer_signal_type;
	typedef boost::signals2::signal< void( LayerHandle, int ) > layer_at_signal_type;
	typedef boost::signals2::signal< void( LayerGroupHandle ) > group_signal_type;
	typedef boost::signals2::signal< void( std::string, int ) > group_at_signal_type;
	typedef boost::signals2::signal< void( std::vector< LayerHandle > ) > layers_signal_type;
	typedef boost::signals2::signal< void() > layers_changed_signal_type;
	typedef boost::signals2::signal< void() > groups_changed_signal_type;
	typedef boost::signals2::signal< void ( std::string, std::string ) > layer_name_changed_signal_type;
	
	// GROUP_INTERNALS_CHANGED_SIGNAL_:
	// signal is triggered when any of the contents of a group change
	group_signal_type group_internals_changed_signal_;
	
	groups_changed_signal_type groups_changed_signal_;
	
	// ACTIVE_LAYER_CHANGED_SIGNAL:
	// This signal is triggered after the active layer is changed
	layer_signal_type active_layer_changed_signal_; 
		
	// LAYER_INSERTED_SIGNAL:
	// This signal is triggered after a layer has been inserted
	layer_signal_type layer_inserted_signal_;

	// LAYERS_DELETED_SIGNAL:
	// This signal is triggered after layers have been deleted and passes a vector of layerhandles
	layers_signal_type layers_deleted_signal_;
		
	// LAYERS_CHANGED_SIGNAL:
	// This gets signaled when layers are inserted, deleted, or reordered.
	layers_changed_signal_type layers_changed_signal_;

	// LAYERS_REORDERED_SIGNAL:
	// Triggered when the order of the layers has been changed.
	layers_changed_signal_type layers_reordered_signal_;

	// LAYER_NAME_CHANGED_SIGNAL:
	// Triggered when the name of a layer has changed.
	// The first parameter is the layer ID, the second is the new name for that layer.
	layer_name_changed_signal_type layer_name_changed_signal_;

	// LAYER_VOLUME_CHANGED_SIGNAL:
	// Triggered when the volume of a layer has changed.
	// The first parameter is the layer handle.
	layer_signal_type layer_volume_changed_signal_;
	
	// LAYER_DATA_CHANGED_SIGNAL:
	/// Trigger when the layer data state is changed. This tracks whether layers
	// are being locked for processing and when new data will be available
	layer_signal_type layer_data_changed_signal_;

protected:
	virtual bool pre_save_states( Core::StateIO& state_io );

	// POST_SAVE_STATES:
	// This function is called after the LayerManagers states have been saved and then
	// tells the groups to save their states as well.
	virtual bool post_save_states( Core::StateIO& state_io );

	// POST_LOAD_STATES:
	// this function creates the layers who's information was saved to file, and then tells them
	// to populate their state variables from file
	virtual bool post_load_states( const Core::StateIO& state_io );
	
	// PRE_LOAD_STATES:
	// this function clears out all existing layers before we load a project from file
	virtual bool pre_load_states( const Core::StateIO& state_io );
	
private:
	friend class LayerManagerPrivate;
	LayerManagerPrivateHandle private_;

	// -- static functions --
public:
	
	// == functions for validation of an action ==
	
	// FINDLAYER:
	// Find a layer inside the layer manager
	static LayerHandle FindLayer( const std::string& layer_id );

	// FINDLAYER:
	// Find a layer inside the layer manager
	static LayerHandle FindLayer( ProvenanceID prov_id );

	// FINDGROUP:
	// Find a layer inside the layer manager
	static LayerGroupHandle FindGroup( const std::string& group_id );

	// FINDGROUP:
	// Find a layer inside the layer manager
	static LayerGroupHandle FindGroup( ProvenanceID prov_id );
	
	// FINDMASKLAYER:
	// Find a mask layer inside the layer manager
	static MaskLayerHandle FindMaskLayer( const std::string& layer_id );

	// FINDDATALAYER:
	// Find a data layer inside the layer manager
	static DataLayerHandle FindDataLayer( const std::string& layer_id );

//// Functions that need to be removed ///////////
	// FINDLAYERGROUP:
	// Find a group inside the layer manager
	// TODO: Need to make this one obsolete for provenance reasons everyhting will have to be
	// done with layerid lists
	// --JGS
	static LayerGroupHandle FindLayerGroup( const std::string& group_id );

	// CHECKGROUPEXISTANCE:
	// Check whether a group exists.
	// If it does not exist, the function returns false and an error is string is returned.
	// TODO: Need to make this one obsolete for provenance reasons everyhting will have to be
	// done with layerid lists
	// --JGS
	static bool CheckGroupExistance( const std::string& layer_id, std::string& error );
//////////////////////////////////////////////////

	// CHECKLAYEREXISTANCE:
	// Check whether a layer exists.
	// If it does not exist, the function returns and reports the error in the context 
	static bool CheckLayerExistance( const std::string& layer_id, 
		Core::ActionContextHandle context );
	
	// Check whether a layer exists.
	// If it does not exist, the function returns false.
	static bool CheckLayerExistance( const std::string& layer_id );	
	
	// CHECKLAYEREXISTANCEANDTYPE:
	// Check whether a layer exists and whether it is of the right type.
	// If it does not exist or is not of the right type, the function returns the error in the
	// context.
	static bool CheckLayerExistanceAndType( const std::string& layer_id, Core::VolumeType type, 
		Core::ActionContextHandle context  );

	// CHECKLAYERSIZE:
	// Check whether a layer has the right size.
	// If it does not have the right size, the function returns false and returns the error in 
	// the context.	
	static bool CheckLayerSize( const std::string& layer_id1, const std::string& layer_id2,
		Core::ActionContextHandle context  );
			
	// CHECKLAYERAVAILABILITYFORPROCESSING:
	// Check whether a layer is available for processing, at the end of the filter the data will
	// be replaced with new data. Hence this is write acces.
	// If a layer is not available a notifier is returned that tells can be used to assess when to
	// check for availability again. Even though the notifier may return another process may have
	// grabbed it in the mean time. In that case a new notifier will need to be issued by rechecking
	// availability. 
	// NOTE: Availability needs to be tested to ensure that another process is not working on this
	// this layer. 
	static bool CheckLayerAvailabilityForProcessing( const std::string& layer_id, 
		Core::ActionContextHandle context  );

	// CHECKLAYERAVAILABILITYFORUSE:
	// Check whether a layer is available for use, i.e. data is not changed but needs to remain
	// unchanged during the process. Hence this is read access
	// If a layer is not available a notifier is returned that tells can be used to assess when to
	// check for availability again. Even though the notifier may return another process may have
	// grabbed it in the mean time. In that case a new notifier will need to be issued by rechecking
	// availability. 
	// NOTE: Availability needs to be tested to ensure that another process is not working on this
	// this layer. 
	static bool CheckLayerAvailabilityForUse( const std::string& layer_id, 
		Core::ActionContextHandle context  );
		
	// CHECKLAYERAVAILABILITY:
	// Check whether a layer is available for use. This case processes both of the above cases:
	// if replace is true, it will check for processing (write) access, if it is not replaced, it 
	// will look for use (read) access
	// If a layer is not available a notifier is returned that tells can be used to assess when to
	// check for availability again. Even though the notifier may return another process may have
	// grabbed it in the mean time. In that case a new notifier will need to be issued by rechecking
	// availability.
	// NOTE: Availability needs to be tested to ensure that another process is not working on this
	// this layer. 
	static bool CheckLayerAvailability( const std::string& layer_id, bool replace,
		Core::ActionContextHandle context  );	
		
	// == functions for creating and locking layers ==
public:	
	// KEY_TYPE:
	// When locking a layer a key is returned. This key keeps track of the asynchronous process
	// and is needed to reinsert a volume into layer. The purpose of the key is to ensure that
	// a volume is only inserted by the asynchronous process if the key matches the one generated
	// when the layer was locked.
	typedef Layer::filter_key_type filter_key_type;

	// These functions can only be called from the application thread
	
	// LOCKFORUSE:
	// Change the layer data_state to IN_USE_C.
	// NOTE: This function can *only* be called from the Application thread.
	static bool LockForUse( LayerHandle layer, filter_key_type key = filter_key_type( 0 ) );
	
	// LOCKFORPROCESSING:
	// Change the layer data_state to PROCESSING_C.
	// NOTE: This function can *only* be called from the Application thread.
	static bool LockForProcessing( LayerHandle layer, filter_key_type key = filter_key_type( 0 ) );
	
	// CREATEANDLOCKMASKLAYER:
	// Create a new mask layer and lock it into the CREATING_C mode.
	// NOTE: This function can *only* be called from the Application thread.
	static bool CreateAndLockMaskLayer( Core::GridTransform transform, const std::string& name, 
		LayerHandle& layer, const LayerMetaData& meta_data, filter_key_type key = 0 );
	
	// CREATEANDLOCKDATALAYER:
	// Create a new data layer and lock it into the CREATING_C mode.
	// NOTE: This function can *only* be called from the Application thread.
	static bool CreateAndLockDataLayer( Core::GridTransform, const std::string& name,
		LayerHandle& layer, const LayerMetaData& meta_data, filter_key_type key = filter_key_type( 0 ) );
	
	// == functions for setting data and unlocking layers ==

	// These functions can be called from the filter thread
	
	// DISPATCHUNLOCKLAYER:
	// Change the layer data_state back to available. This function will relay a call to the 
	// Application thread if needed.
	static void DispatchUnlockLayer( LayerHandle layer, filter_key_type key = filter_key_type( 0 ) );

	// DISPATCHDELETELAYER:
	// Delete the layer. This function will relay a call to the 
	// Application thread if needed.
	static void DispatchDeleteLayer( LayerHandle layer, filter_key_type key = filter_key_type( 0 ) );
	
	// DISPATCHUNLOCKORDELETELAYER:
	// Unlock layer if valid, delete otherwise. This function will relay a call to the 
	// Application thread if needed.
	static void DispatchUnlockOrDeleteLayer( LayerHandle layer, 
		filter_key_type key = filter_key_type( 0 ) );

	// DISPATCHINSERTDATAVOLUMEINTOLAYER:
	// Insert a data volume into a data layer. This function will relay a call to the 
	// Application thread if needed.
	static void DispatchInsertDataVolumeIntoLayer( DataLayerHandle layer, 
		Core::DataVolumeHandle data, ProvenanceID provid, 
		filter_key_type key = filter_key_type( 0 ) );

	// DISPATCHINSERTMASKVOLUMEINTOLAYER:
	// Insert a mask volume into a mask layer. This function will relay a call to the 
	// Application thread if needed.
	static void DispatchInsertMaskVolumeIntoLayer( MaskLayerHandle layer, 
		Core::MaskVolumeHandle mask, ProvenanceID provid, 
		filter_key_type key = filter_key_type( 0 ) );

	// DISPATCHINSERTVOLUMEINTOLAYER:
	// Insert a mask or data volume into a layer. This function will relay a call to the 
	// Application thread if needed.
	static void DispatchInsertVolumeIntoLayer( LayerHandle layer, 
		Core::VolumeHandle mask, ProvenanceID provid, 
		filter_key_type key = filter_key_type( 0 ) );

	// DISPATCHINSERTDATASLICEINTOLAYER:
	// Insert a data slice into a data layer. 
	static void DispatchInsertDataSliceIntoLayer( DataLayerHandle layer,
		Core::DataSliceHandle data, ProvenanceID provid, 
		filter_key_type key = filter_key_type( 0 ) );

	// DISPATCHINSERTDATASLICEINTOLAYER:
	// Insert a data slice into a data layer. 
	static void DispatchInsertDataSlicesIntoLayer( DataLayerHandle layer,
		std::vector<Core::DataSliceHandle> data, ProvenanceID provid, 
		filter_key_type key = filter_key_type( 0 ) );

	// DISPATCHINSERTMASKSLICEINTOLAYER:
	// Insert a data slice into a data layer. 
	static void DispatchInsertMaskSliceIntoLayer( MaskLayerHandle layer,
		Core::MaskDataSliceHandle mask, ProvenanceID provid,
		filter_key_type key = filter_key_type( 0 ) );

	// DISPATCHINSERTMASKSLICESINTOLAYER:
	// Insert a data slice into a data layer. 
	static void DispatchInsertMaskSlicesIntoLayer( MaskLayerHandle layer,
		std::vector<Core::MaskDataSliceHandle> mask, ProvenanceID provid, 
		filter_key_type key = filter_key_type( 0 ) );

	// -- functions for obtaining the current layer and group id counters --
	typedef std::vector<int> id_count_type;
	
	// GETLAYERIDCOUNT:
	// Get the current count of the group and layer ids
	static id_count_type GetLayerIdCount();

	// GETLAYERINVALIDIDCOUNT:
	// Get a default id count that has no valid ids.
	static id_count_type GetLayerInvalidIdCount();
	
	// SETLAYERIDCOUNT:
	// Set the current count of group and layer
	// NOTE: This function should only be called by the undo buffer
	static void SetLayerIdCount( id_count_type id_count );

};

} // end namespace seg3D

#endif
