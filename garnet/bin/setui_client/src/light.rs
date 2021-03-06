// Copyright 2020 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

use anyhow::{format_err, Error};
use fidl_fuchsia_settings::{LightProxy, LightState};

use crate::LightGroup;

pub async fn command(proxy: LightProxy, light_group: LightGroup) -> Result<String, Error> {
    let has_name = light_group.name.is_some();
    let has_values =
        light_group.simple.len() + light_group.brightness.len() + light_group.rgb.len() > 0;

    if !has_name && !has_values {
        // No values set, perform a watch instead.
        let setting_value = proxy.watch_light_groups().await?;
        return Ok(format!("{:#?}", setting_value));
    }

    if !has_values {
        // Only name specified, perform watch on individual light group.
        let setting_value = proxy.watch_light_group(light_group.name.unwrap().as_str()).await?;
        return Ok(format!("{:#?}", setting_value));
    }

    if !has_name {
        return Err(format_err!("light group name required"));
    }

    let light_states: Vec<LightState> = light_group.clone().into();

    let light_state_str = format!("{:?}", light_states);
    let result = proxy
        .set_light_group_values(
            light_group.name.clone().unwrap().as_str(),
            &mut light_states.into_iter(),
        )
        .await?;
    return match result {
        Ok(_) => Ok(format!(
            "Successfully set light group {} with values {:?}",
            light_group.name.unwrap(),
            light_state_str
        )),
        Err(err) => Ok(format!("{:#?}", err)),
    };
}
