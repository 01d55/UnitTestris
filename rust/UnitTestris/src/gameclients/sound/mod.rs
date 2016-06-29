extern crate portaudio;
extern crate vorbis;

pub use self::portaudio::PortAudio;

mod file {
    use super::vorbis::*;
    use std::path::Path;
    use std::fs::File;
    use std::io::{Read, Seek};

    pub const MUSIC_PATH: &'static str = "../../sound/music.ogg";
    pub fn open_file(path: &Path) -> Decoder<File> {
        let file = File::open(path).unwrap();
        Decoder::new(file).unwrap()
    }
    pub fn read_consolidated_packet<R: Read + Seek>(decoder: Decoder<R>) -> Option<Packet> {
        let mut iter = decoder.into_packets().map(|item| { item.unwrap() });
        match iter.next() {
            None => None,
            Some(mut first_packet) => {
                for packet in iter {
                    first_packet.data.extend(packet.data);
                    assert_eq!((first_packet.channels,
                                first_packet.rate,
                                first_packet.bitrate_upper,
                                first_packet.bitrate_nominal,
                                first_packet.bitrate_lower,
                                first_packet.bitrate_window),
                               (packet.channels,
                                packet.rate,
                                packet.bitrate_upper,
                                packet.bitrate_nominal,
                                packet.bitrate_lower,
                                packet.bitrate_window));
                }
                Some(first_packet)
            }
        }
    }
}

/// 'pa is lifetime of a PortAudio
pub struct Player<'pa> {
    stream: Option<portaudio::Stream<'pa, portaudio::NonBlocking, portaudio::Output<i16>>>,
}

impl<'pa> Player<'pa> {

    pub fn new() -> Self {
        Player {
            stream: None,
        }
    }
    /// Starts music if it is stopped or paused. Otherwise pauses.
    pub fn toggle_play_music(&mut self, pa: &'pa PortAudio) -> Result<(), portaudio::Error> {
        // as_ref borrows self & self.stream too long
        if let Some(mut stream) = self.stream.take() {
            if try!(stream.is_active()) {
                try!(stream.stop());
            } else {
                try!(stream.start());
            }
            self.stream = Some(stream);
            Ok(())
        } else {
            use std::path::Path;
            let packet = file::read_consolidated_packet(file::open_file(Path::new(file::MUSIC_PATH))).unwrap();
            let (data, out_settings, channels) = (
                packet.data,
                try!(pa.default_output_stream_settings(
                    packet.channels as i32, // from u16
                    packet.rate as f64, // from u64
                    0 // "The special value paFramesPerBufferUnspecified (0) may be used to request that the stream callback will receive an optimal (and possibly varying) number of frames based on host requirements and the requested latency settings."
                    // "Note: With some host APIs, the use of non-zero framesPerBuffer for a callback stream may introduce an additional layer of buffering which could introduce additional latency."
                )),
                packet.channels as usize
            );
            let mut data_idx = 0;
            // high priority - no blocking (e.g. mutex locks) allowed
            let callback = move |portaudio::OutputStreamCallbackArgs { buffer, frames, .. }| {
                let mut buffer_idx = 0;
                for _ in 0..frames {
                    for channel in 0..channels {
                        buffer[buffer_idx + channel] = data[(data_idx+channel)%data.len()];
                    }
                    buffer_idx += channels;
                    data_idx = (data_idx + channels)%data.len();
                }
                portaudio::Continue
            };
            let mut stream = try!(pa.open_non_blocking_stream(out_settings, callback));
            try!(stream.start());
            self.stream = Some(stream);
            Ok(())
        }
    }
    /// Start music if it is not currently playing. Otherwise does nothing.
    pub fn start_music(&mut self, pa: &'pa PortAudio) -> Result<(), portaudio::Error> {
        // self.stream.as_mut borrows self too long
        if let Some(stream) = self.stream.take() {
            if ! try!(stream.is_active()) {
                self.stream = Some(stream);
                self.toggle_play_music(pa)
            } else {
                self.stream = Some(stream);
                Ok(())
            }
        } else {
            Ok(())
        }
    }
    /// Pause music if it is currently playing. Otherwise does nothing.
    pub fn pause_music(&mut self, pa: &'pa PortAudio) -> Result<(), portaudio::Error> {
        // self.stream.as_mut borrows self too long
        if let Some(stream) = self.stream.take() {
            if try!(stream.is_active()) {
                self.stream = Some(stream);
                self.toggle_play_music(pa)
            } else {
                self.stream = Some(stream);
                Ok(())
            }
        } else {
            Ok(())
        }
    }
    /// Resets music if it is currently playing or paused. Otherwise does nothing.
    pub fn reset_music(&mut self) -> Result<(), portaudio::Error> {
        if let Some(mut stream) = self.stream.take() {
            try!(stream.stop());
            stream.close()
        } else {
            Ok(())
        }
    }
    pub fn is_playing(&self) -> Result<bool, portaudio::Error> {
        if let Some(stream) = self.stream.as_ref() {
            stream.is_active()
        } else {
            Ok(false)
        }
    }
}
