(function() {
    var knob = document.getElementById('knob');
    var knobAngle = 0;

    document.addEventListener('keydown', function(e) {
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
        if (e.key === ' ') {
            knob.classList.remove('pressed');
        }
    });

    document.addEventListener('wheel', function(e) {
        var delta = e.deltaY > 0 ? -1 : 1;
        Module.encoderDelta = (Module.encoderDelta || 0) + delta;
        knobAngle -= delta * 15;
        knob.style.transform = 'rotate(' + knobAngle + 'deg)';
        e.preventDefault();
    }, { passive: false });
})();
