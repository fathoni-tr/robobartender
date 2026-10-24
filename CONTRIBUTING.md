# Contributing to ROBOBARTENDER

Thanks for wanting to improve the robot. This project is small on purpose — one sketch, one machine — so contributions are easiest to merge when they stay that way.

By participating you agree to the [Code of Conduct](CODE_OF_CONDUCT.md).

## Ways to contribute

- **Bug reports** — wrong pour timing, servo jitter, LCD or I2C failures.
- **Hardware variants** — different board, pump count, or servo layout.
- **Docs** — wiring diagrams, photos of your build, translation.
- **Code** — new flavors, safety checks, non-blocking sequences.

## Reporting a bug

Open an issue and include:

1. Board and core version (`Tools → Board`, `arduino-cli core list`)
2. Library versions for `LiquidCrystal_I2C` and `Adafruit PWM Servo Driver Library`
3. Your wiring for the affected pins, and whether servo power is separate
4. What you expected vs what happened
5. The full serial output if you added `Serial` prints, or a video for mechanical problems

**Do not** report security-sensitive issues (for example a way to make pumps run unattended) in a public issue — contact the maintainer privately first.

## Development setup

```bash
git clone https://github.com/fathoni-tr/robobartender.git
cd robobartender
arduino-cli core install esp32:esp32
arduino-cli lib install "LiquidCrystal I2C" "Adafruit PWM Servo Driver Library"
```

Verify it compiles before opening a pull request:

```bash
arduino-cli compile --fqbn esp32:esp32:esp32 .
```

No unit-test framework is used — hardware behavior is verified on a real machine. If your change touches the pour sequence, state in the PR which hardware you ran it on and with what liquid.

## Pull requests

1. Fork, branch from `master` (`git checkout -b feat/label`).
2. Keep the diff focused — one concern per PR.
3. Match the existing style: `const int` for pins, `#define` for servo channels, comments in the language already used in that block.
4. Do not renumber existing pins or PCA9685 channels without a migration note — people have machines already wired.
5. Keep relay-off-on-boot behavior and any safety delays intact.
6. Describe hardware testing in the PR body, or say plainly that it is compile-tested only.
7. Reference related issues (`Fixes #12`).

Maintainer will review; expect questions about the physical behavior of your change.

## Commit messages

Conventional-commit style, imperative mood:

```
feat: add fourth flavor channel
fix: keep relays off when choice is unset
docs: note active-low relay assumption
```

## Coding guidelines

- One `.ino` file until it genuinely must split; then move hardware helpers to `hardware.h`.
- Avoid blocking `delay()` for anything longer than servo settling — prefer `millis()` state machines if you rework the sequence.
- All timing that a user might tune gets a named constant or a clear comment.
- New user-visible strings appear on the 16x2 LCD: keep them ≤16 characters per line.

## License

By contributing you agree that your contributions are licensed under the [MIT License](LICENSE).
