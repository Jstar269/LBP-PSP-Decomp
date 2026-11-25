#!/usr/bin/env python3
"""Tests for the retail string-reference scanner."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lbpstrings as S  # noqa: E402


def i_type(op: int, rs: int, rt: int, imm: int) -> bytes:
    return ((op << 26) | (rs << 21) | (rt << 16) | (imm & 0xFFFF)).to_bytes(4, "little")


def r_type(rs: int, rt: int, rd: int, funct: int) -> bytes:
    return ((rs << 21) | (rt << 16) | (rd << 11) | funct).to_bytes(4, "little")


class StringReferenceTests(unittest.TestCase):
    def test_string_literal_reference(self) -> None:
        data = bytearray(0x60)
        data[0x40:0x45] = b"TEXT\0"
        # lui t0, 0; addiu a0, t0, 0x40
        code = i_type(0x0F, 0, 8, 0) + i_type(0x09, 8, 4, 0x40)
        data[0x10:0x18] = code
        self.assertEqual(S.strings_in(bytes(data), {"start": 0, "file_offset": 0, "file_end": len(data)},
                                      0x10, 0x18), [(0x14, "TEXT")])

    def test_r_type_write_invalidates_stale_lui_value(self) -> None:
        data = bytearray(0x60)
        data[0:5] = b"FAKE\0"
        # lui t0, 0; addu t0, t1, t2; addiu a0, t0, 0
        # The old high half no longer describes t0 after addu.
        code = i_type(0x0F, 0, 8, 0) + r_type(9, 10, 8, 0x21) + i_type(0x09, 8, 4, 0)
        data[0x10:0x1C] = code
        self.assertEqual(S.strings_in(bytes(data), {"start": 0, "file_offset": 0, "file_end": len(data)},
                                      0x10, 0x1C), [])

    def test_immediate_write_invalidates_stale_lui_value(self) -> None:
        data = bytearray(0x60)
        data[0:5] = b"FAKE\0"
        # lui t0, 0; xori t0, t1, 1; addiu a0, t0, 0
        code = i_type(0x0F, 0, 8, 0) + i_type(0x0E, 9, 8, 1) + i_type(0x09, 8, 4, 0)
        data[0x10:0x1C] = code
        self.assertEqual(S.strings_in(bytes(data), {"start": 0, "file_offset": 0, "file_end": len(data)},
                                      0x10, 0x1C), [])

    def test_lui_does_not_track_zero_register(self) -> None:
        data = bytearray(0x60)
        data[0:5] = b"ZERO\0"
        # lui $zero, 0; addiu a0, $zero, 0 must not invent a reference to address zero.
        code = i_type(0x0F, 0, 0, 0) + i_type(0x09, 0, 4, 0)
        data[0x10:0x18] = code
        self.assertEqual(S.strings_in(bytes(data), {"start": 0, "file_offset": 0, "file_end": len(data)},
                                      0x10, 0x18), [])

    def test_syscall_code_bits_do_not_clear_tracked_gpr(self) -> None:
        data = bytearray(0x60)
        data[0:5] = b"TEXT\0"
        # syscall with code 0x20 has nonzero bits in the decoded rd field, but writes no GPR.
        code = i_type(0x0F, 0, 8, 0) + (0x0000080C).to_bytes(4, "little") + i_type(0x09, 8, 4, 0)
        data[0x10:0x1C] = code
        self.assertEqual(S.strings_in(bytes(data), {"start": 0, "file_offset": 0, "file_end": len(data)},
                                      0x10, 0x1C), [(0x18, "TEXT")])


if __name__ == "__main__":
    unittest.main()
