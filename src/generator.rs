use anyhow::Ok;
use askama::Template;


#[derive(Template)]
#[template(path = "pac.hpp", escape = "none")]
pub struct PeripheralTemplate<'a> {
    peripheral: &'a Peripheral,
}

pub struct Peripheral {
    name: String,
    base_address: u64,
    registers: Vec<Register>,
}

pub struct Register {
    name: String,
    address_offset: u32,
    reset_value: Option<u32>,
    fields: Vec<Field>,
}

pub struct Field {
    name: String,
    bit_offset: u32,
    bit_width: u32,
    enumerated_values: Option<Vec<EnumValue>>,
}

pub struct EnumValue {
    name: String,
    value: u32,
    description: Option<String>,
}

pub fn generate_pac(device: &svd_rs::Device, output_folder: &str) -> Result<(), anyhow::Error> {
    for peripheral in &device.peripherals {
        let p = Peripheral {
            name: peripheral.name.clone(),
            base_address: peripheral.base_address,
            registers: Vec::new(),
        };
        let filepath = format!("{}/{}.hpp", output_folder, p.name);
        generate_pack_file(&p, &filepath)?;
    }

    Ok(())
}

fn generate_pack_file(peripheral: &Peripheral, filepath: &str) -> Result<(), anyhow::Error> {
    let template = PeripheralTemplate { peripheral };
    let rendered = template.render()?;
    std::fs::write(filepath, rendered)?;
    Ok(())
}