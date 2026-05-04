(function() {
    var knob = document.getElementById('knob');
    var knobAngle = 0;

    function isTextInputFocused() {
        var el = document.activeElement;
        return el && (el.tagName === 'TEXTAREA' || el.tagName === 'INPUT');
    }

    document.addEventListener('keydown', function(e) {
        if (isTextInputFocused()) return;

        if (e.key === 'ArrowUp') {
            Module.encoderDelta = (Module.encoderDelta || 0) + 1;
            knobAngle -= 15;
            knob.style.transform = 'rotate(' + knobAngle + 'deg)';
            e.preventDefault();
        } else if (e.key === 'ArrowDown') {
            Module.encoderDelta = (Module.encoderDelta || 0) - 1;
            knobAngle += 15;
            knob.style.transform = 'rotate(' + knobAngle + 'deg)';
            e.preventDefault();
        } else if (e.key === ' ') {
            Module.buttonPressed = true;
            knob.classList.add('pressed');
            e.preventDefault();
        }
    });

    document.addEventListener('keyup', function(e) {
        if (isTextInputFocused()) return;

        if (e.key === ' ') {
            knob.classList.remove('pressed');
        }
    });

    document.addEventListener('wheel', function(e) {
        if (isTextInputFocused()) return;

        var delta = e.deltaY > 0 ? -1 : 1;
        Module.encoderDelta = (Module.encoderDelta || 0) + delta;
        knobAngle -= delta * 15;
        knob.style.transform = 'rotate(' + knobAngle + 'deg)';
        e.preventDefault();
    }, { passive: false });

    // --- Text loading ---

    function loadTextIntoWasm(text) {
        if (!text || !text.trim()) return;
        var len = Module.lengthBytesUTF8(text);
        Module.ccall('setReaderText', null, ['string', 'number'], [text, len]);
    }

    function handleFile(file) {
        if (!file) return;
        var reader = new FileReader();
        reader.onload = function(e) {
            loadTextIntoWasm(e.target.result);
        };
        reader.readAsText(file);
    }

    var dropZone = document.getElementById('drop-zone');
    var fileInput = document.getElementById('file-input');
    var textInput = document.getElementById('text-input');
    var loadBtn = document.getElementById('load-text-btn');

    dropZone.addEventListener('click', function() {
        fileInput.click();
    });

    fileInput.addEventListener('change', function() {
        if (fileInput.files.length > 0) {
            handleFile(fileInput.files[0]);
            fileInput.value = '';
        }
    });

    dropZone.addEventListener('dragover', function(e) {
        e.preventDefault();
        dropZone.classList.add('drag-over');
    });

    dropZone.addEventListener('dragleave', function() {
        dropZone.classList.remove('drag-over');
    });

    dropZone.addEventListener('drop', function(e) {
        e.preventDefault();
        dropZone.classList.remove('drag-over');
        if (e.dataTransfer.files.length > 0) {
            handleFile(e.dataTransfer.files[0]);
        }
    });

    loadBtn.addEventListener('click', function() {
        loadTextIntoWasm(textInput.value);
    });
})();
