# External Submodules

 This repository uses Git submodules to incorporate standards implementations.

Submodules:

- AES3-2009: path `external/AES3-2009`, upstream <https://github.com/zarfld/AES3-2009>
  - Pinned SHA: `126b6593` (see `git submodule status`)
  - Purpose: AES3 frame/preamble and transport utilities used by AES-11.
- AES5-2018: path `external/AES5-2018`, upstream <https://github.com/zarfld/AES5-2018>
  - Pinned SHA: `1f356f8e` (see `git submodule status`)
  - Purpose: Preferred sampling frequency definitions and validators (adapter planned).

Update policy:

- Submodules are pinned to immutable SHAs for reproducible builds. Do not track branches.
- To bump a submodule:
  1. Enter the submodule folder, fetch upstream, checkout desired commit/tag.
  2. Return to root and commit the updated gitlink.
  3. Run full test suite.

 CI reminder: Ensure CI checks out submodules recursively before building, e.g. `git submodule update --init --recursive`.

Adapter layer:

- AES5 is consumed via an internal adapter (`AES::AES11::_2009::core::AES5Adapter`).
- Direct includes of submodule headers in public headers are forbidden; wiring occurs in cpp via adapter only.
