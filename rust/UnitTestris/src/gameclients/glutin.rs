use glium::{
    Frame,
    IndexBuffer,
    Surface,
    VertexBuffer,
    glutin,
    program,
    texture,
};
use glium::index::PrimitiveType;
use glium::backend::glutin_backend::GlutinFacade;

#[derive(Copy, Clone)]
// Non-snake case names mandated by shaders
#[allow(non_snake_case)]
struct Vertex {
    InVertex: [f32; 3],
    InTexCoord0: [f32; 2]
}
implement_vertex!(Vertex, InVertex, InTexCoord0);
// clockwise order
const SQUARE_VERTICES: [Vertex; 4] = [
    Vertex {
        InVertex: [ 0.5f32, 0.5f32, 0.0f32],
        InTexCoord0: [1.0f32, 1.0f32]
    },
    Vertex {
        InVertex: [ 0.5f32,-0.5f32, 0.0f32],
        InTexCoord0: [1.0f32, 0.0f32]
    },
    Vertex {
        InVertex: [-0.5f32,-0.5f32, 0.0f32],
        InTexCoord0: [0.0f32, 0.0f32]
    },
    Vertex {
        InVertex: [-0.5f32, 0.5f32, 0.0f32],
        InTexCoord0: [0.0f32, 1.0f32]
    },
];
const SQUARE_INDICES: [u16; 6] = [
    0,3,1,
    2,1,3
];

/// Runs tetris in a GL context managed by the glium library
pub fn run_tetris() {
    use super::super::gamestate::*;
    use super::RenderData;
    use glium::DisplayBuild;
    use std::sync::{Arc,Mutex,MutexGuard};
    use std::cell::Ref;
    use std::convert::From;
    let display = glutin::WindowBuilder::new()
        .build_glium()
        .unwrap();
    let vertex_buffer = VertexBuffer::new(&display, &SQUARE_VERTICES).unwrap();
    let index_buffer = IndexBuffer::new(&display, PrimitiveType::TrianglesList, &SQUARE_INDICES).unwrap();
    let program = load_shaders(&display).unwrap();
    let image = gen_image();
    let texture = texture::Texture2d::new(&display, image).unwrap();

    let data_mutex: Arc<Mutex<Option<RenderData>>> = Arc::new(Mutex::new(None));
    let mut game: game::Game;
    {
        let data_mutex = data_mutex.clone();
        let callback = move |field: Ref<field::Field>, current: &piece::Piece, ghost: Option<&piece::Piece>| {
            let new_data = RenderData {
                current: From::from(current),
                field: field.clone(),
                ghost: match ghost {
                    Some(piece) => Some(From::from(piece)),
                    None => None
                },
            };
            let mut data: MutexGuard<Option<super::RenderData>> = data_mutex.lock().unwrap();
            *data = Some(new_data);
        };
        game = game::Game::new(Box::new(callback));
    }
    game.run().unwrap();
    loop {
        // TODO: input
        for ev in display.poll_events() {
            match ev {
                glutin::Event::Closed => return,
                _ => ()
            }
        }
        let data_opt: Option<RenderData> = data_mutex.lock().unwrap().clone();
        if let Some(data) = data_opt {
            draw_frame(&display, data, &vertex_buffer, &index_buffer, &program, &texture);
        }
    }
}

fn load_shaders(display: &GlutinFacade) -> Result<program::Program, program::ProgramCreationError> {
    use std::fs::*;
    use std::io::Read;
    use std::path::Path;
    const VERT_PATH: &'static str = "shaders/simple.vert";
    const FRAG_PATH: &'static str = "shaders/default.frag";
    const PROJECT_ROOT: &'static str = "../../";

    let vert_path = Path::new(PROJECT_ROOT).join(Path::new(VERT_PATH));
    let frag_path = Path::new(PROJECT_ROOT).join(Path::new( FRAG_PATH));
    let mut vertex_file: File = File::open(vert_path.clone()).expect(&format!("couldn't open {:?}", vert_path));
    let mut frag_file: File = File::open(frag_path.clone()).expect(&format!("couldn't open {:?}", frag_path));
    let mut vertex_str = String::new();
    let mut frag_str = String::new();

    vertex_file.read_to_string(&mut vertex_str).expect(&format!("couldn't read vertex_file {:?}", vertex_file));
    frag_file.read_to_string(&mut frag_str).expect(&format!("couldn't read frag_file {:?}", frag_file));
    program::Program::from_source(display, &vertex_str, &frag_str, None)
}


fn draw_block<V:Copy>(target: &mut Frame,
                           v_buff: &VertexBuffer<V>, i_buff: &IndexBuffer<u16>,
                           program: &program::Program,
                           offset: (f32, f32), color: (f32, f32, f32, f32), tex: &texture::Texture2d) -> ()
{
    let uniforms = uniform! {
        offset: offset,
        tint: color,
        msampler: tex,
    };
    target.draw(v_buff, i_buff, program, &uniforms, &Default::default()).unwrap();
}

fn draw_frame<V: Copy>(display: &GlutinFacade,
                        data: super::RenderData,
                        v_buff: &VertexBuffer<V>, i_buff: &IndexBuffer<u16>,
                        program: &program::Program,
                        tex: &texture::Texture2d) -> ()
{
    use super::super::gamestate::field;
    use super::super::gamestate::piece::Type;
    use super::super::gamestate::Coord;
    //const WHITE_COLOR: (f32, f32, f32, f32) = (1.0f32, 1.0f32, 1.0f32, 1.0f32);
    const GREY_COLOR: (f32, f32, f32, f32) = (0.6f32, 0.6f32, 0.6f32, 0.6f32);
    const I_COLOR: (f32, f32, f32, f32) = (1.0f32, 0.0f32, 0.0f32, 1.0f32);
    const J_COLOR: (f32, f32, f32, f32) = (0.0f32, 1.0f32, 0.0f32, 1.0f32);
    const L_COLOR: (f32, f32, f32, f32) = (0.0f32, 0.0f32, 1.0f32, 1.0f32);
    const O_COLOR: (f32, f32, f32, f32) = (1.0f32, 1.0f32, 0.0f32, 1.0f32);
    const S_COLOR: (f32, f32, f32, f32) = (1.0f32, 0.0f32, 1.0f32, 1.0f32);
    const T_COLOR: (f32, f32, f32, f32) = (0.0f32, 1.0f32, 1.0f32, 1.0f32);
    const Z_COLOR: (f32, f32, f32, f32) = (0.5f32, 1.0f32, 0.5f32, 1.0f32);

    let mut frame: Frame = display.draw();
    frame.clear_color(0.0, 0.0, 0.0, 1.0);
    // Draw blocks in field
    for i in 0..field::WIDTH {
        for j in 0..field::HEIGHT {
            if data.field.get(Coord::new(i as i32, j as i32)).unwrap() {
                draw_block(&mut frame, v_buff, i_buff, program,
                           (i as f32, j as f32),
                           GREY_COLOR, tex);
            }
        }
    }
    // Draw current piece
    let color = match data.current.typ {
        Type::I => I_COLOR,
        Type::J => J_COLOR,
        Type::L => L_COLOR,
        Type::O => O_COLOR,
        Type::S => S_COLOR,
        Type::T => T_COLOR,
        Type::Z => Z_COLOR
    };
    for block in &data.current.blocks {
        draw_block(&mut frame, v_buff, i_buff, program,
                   (block.x as f32, block.y as f32),
                   color, tex);
    }
    frame.finish().unwrap();
}

fn gen_image() -> texture::RawImage2d<'static, (f32, f32, f32)> {
    use std::cmp::min;
    use std::mem;
    use glium::texture::ClientFormat;
    const TEXDIMENSION: u32 = 256;
    const TEXBUFFSIZE: usize = (TEXDIMENSION*TEXDIMENSION) as usize;
    let mut raw_data: Vec<(f32, f32, f32)> = Vec::new();
    for i in 0..TEXDIMENSION {
        for j in 0..TEXDIMENSION {
            let idist = min(TEXDIMENSION-i, i);
            let jdist = min(TEXDIMENSION-j, j);
            let dist = min(idist, jdist);
            let value: f32 = (dist as f32) / (TEXDIMENSION as f32) + 0.5f32;
            raw_data.push((value, value, value));
        }
    }
    assert_eq!(raw_data.len(), TEXBUFFSIZE);
    let mut image = texture::RawImage2d::from_raw_rgb(raw_data, (TEXDIMENSION, TEXDIMENSION));
    match image.format {
        ClientFormat::F32F32F32 => (),
        _ => {
            println!("correcting wrong format: {:?}", image.format);
            image.format = ClientFormat::F32F32F32;
        }
    }
    assert!(image.data.len() == image.width as usize * image.height as usize * image.format.get_size() / mem::size_of::<(f32, f32, f32)>(),
            "size mismatch: len {:?}, width {:?}, height {:?}, get_size() {:?}, size_of {:?}",
            image.data.len(), image.width, image.height, image.format.get_size(), mem::size_of::<(f32, f32, f32)>());
    image
}
