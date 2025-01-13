/********************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
********************************************************************************/
#pragma once

namespace Presets {
namespace Clusters {

/******************************************************
 *               Preset Clusters
 ******************************************************/
void matter_cluster_descriptor_server(ClusterConfig *clusterConfig);
void matter_cluster_acl_server(ClusterConfig *clusterConfig);
void matter_cluster_basic_information_server(ClusterConfig *clusterConfig);
void matter_cluster_ota_requestor_server(ClusterConfig *clusterConfig);
void matter_cluster_general_commissioning_server(ClusterConfig *clusterConfig);
void matter_cluster_network_commissioning_server(ClusterConfig *clusterConfig);
void matter_cluster_general_diagnostics_server(ClusterConfig *clusterConfig);
void matter_cluster_software_diagnostics_server(ClusterConfig *clusterConfig);
void matter_cluster_wifi_diagnostics_server(ClusterConfig *clusterConfig);
void matter_cluster_administrator_commissioning_server(ClusterConfig *clusterConfig);
void matter_cluster_operational_credentials_server(ClusterConfig *clusterConfig);
void matter_cluster_group_key_management_server(ClusterConfig *clusterConfig);
void matter_cluster_identify_server(ClusterConfig *clusterConfig);
void matter_cluster_groups_server(ClusterConfig *clusterConfig);
void matter_cluster_scenes_server(ClusterConfig *clusterConfig);
void matter_cluster_onoff_server(ClusterConfig *clusterConfig);
void matter_cluster_level_control_server(ClusterConfig *clusterConfig);
void matter_cluster_thermostat_server(ClusterConfig *clusterConfig);
void matter_cluster_thermostat_ui_server(ClusterConfig *clusterConfig);
void matter_cluster_fan_control_server(ClusterConfig *clusterConfig);
void matter_cluster_temperature_measurement_server(ClusterConfig *clusterConfig);
void matter_cluster_relative_humidity_measurement_server(ClusterConfig *clusterConfig);
} // Clusters

namespace Endpoints {

/******************************************************
 *               Preset Device types (Endpoint)
 ******************************************************/
void matter_root_node_preset(EndpointConfig *rootNodeEndpointConfig);
void matter_dimmable_light_preset(EndpointConfig *dimmableLightEndpointConfig);
void matter_fan_preset(EndpointConfig *FanEndpointConfig);
void matter_thermostat_preset(EndpointConfig *ThermostatEndpointConfig);
void matter_room_air_conditioner_preset(EndpointConfig *RoomAirConEndpointConfig);
void matter_aggregator_preset(EndpointConfig *aggregatorEndpointConfig);

} // Endpoints
} // Presets
