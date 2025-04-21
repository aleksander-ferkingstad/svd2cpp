use std::fs;
use std::env::args;

mod generator;
mod svd_file;

fn main() -> Result<(), Box<dyn std::error::Error>> {

    let args: Vec<String> = args().collect();
    if args.len() < 2 {
        eprintln!("Usage: {} <input_file>", args[0]);
        std::process::exit(1);
    }
    
    if fs::metadata("generated").is_ok() {
        fs::remove_dir_all("generated")?;
    }

    fs::create_dir("generated")?;
    fs::create_dir("generated/headers")?;
    fs::copy("src/register.hpp", "generated/headers/register.hpp")?;

    
    let device = svd_file::parse_svd_file(&args[1])?;
    let json_file = format!("generated/{}.json", device.name.clone());
    svd_file::svd_device_to_json_file(&device, &json_file)?;

    generator::generate_pac(&device, "generated/headers")?;
    
    return Ok(());
}
