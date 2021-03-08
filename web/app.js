Module.onRuntimeInitialized = async _ => {
    const c = document.getElementById("chip8-canvas");
    const context = c.getContext("2d");
    const PERIOD = 4.0;

    const CHIP8 = {
        initialize: Module.cwrap('CHIP8_wasm_initialize', 'number', ['number', 'number']),
        tick: Module.cwrap('CHIP8_wasm_tick', 'number', []),
        get_video: Module.cwrap('CHIP8_wasm_get_video', 'number', []),
        get_PC: Module.cwrap('CHIP8_wasm_get_PC', 'number', []),
        get_register_file: Module.cwrap('CHIP8_wasm_get_register_file', 'number', []),
        set_key_down: Module.cwrap('CHIP8_wasm_set_key_down', '', ['number']),
        set_key_up: Module.cwrap('CHIP8_wasm_set_key_up', '', ['number'])
    };

    CHIP8.initialize();

    const videoAddr = CHIP8.get_video();
    const video = Module.HEAPU8.subarray(videoAddr, videoAddr + 64 * 32);

    const registerAddr = CHIP8.get_register_file();
    const registerFile = Module.HEAPU8.subarray(registerAddr, registerAddr + 16);

    document.onkeydown = (e) => CHIP8.set_key_down(e.key.toLowerCase().charCodeAt(0));
    document.onkeyup = (e) => CHIP8.set_key_up(e.key.toLowerCase().charCodeAt(0));

    function ticker() {
        let old_timestamp = null;
        let registerPCElement = document.getElementById("chip8-PC");
        let registerFileElements = Array.from({length: 16}, (x, i) => {
            return document.getElementById("reg-v" + i.toString(16).toUpperCase());
        });

        context.fillStyle = "white";

        function tick(ts) {
            let n = old_timestamp && Math.floor((ts - old_timestamp) / PERIOD);
            if (!old_timestamp) {
                old_timestamp = ts;
            } else {
                old_timestamp += n * PERIOD;
            }

            let draw_flag = 0;
            for (let i = 0; i < n; i++) {
                draw_flag |= CHIP8.tick();
            }

            registerPCElement.textContent = CHIP8.get_PC();

            for (let i = 0; i < 16; i++) {
                registerFileElements[i].textContent = registerFile[i].toString();
            }

            if (draw_flag) {
                context.clearRect(0, 0, c.width, c.height);

                context.beginPath();
                for (let y = 0; y < 32; y++) {
                    for (let x = 0; x < 64; x++) {
                        if (video[y * 64 + x]) {
                            context.fillRect(x * 10, y * 10, 10, 10);
                        }
                    }
                }
                context.closePath();
            }

            requestAnimationFrame(tick);
        }

        return tick;
    }

    requestAnimationFrame(ticker());
};