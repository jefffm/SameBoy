"""Focused CH4 DAC-off phase/duration probes; the caller finishes each Case.

Observe post-trigger audio phase, not hidden LFSR evolution. These cases carry
activity expectations, not a universal hardware phase PASS/FAIL criterion.
"""
from cases import Case, FRAME


def noise_cases():
    """Return 54 unfinished cases: 18 DAC-off and 9 DAC-on per noise width."""
    treatments = [
        (pre, off, 0)
        for pre in (0, 384, 768, 1152)
        for off in (0, 384, 1536)
    ] + [
        (pre, 1536, post)
        for pre in (0, 768)
        for post in (384, 768, 1536)
    ]
    controls = [(pre, 1536, 0) for pre in (0, 384, 768, 1152)] + [
        (0, off, 0) for off in (0, 384)
    ] + [
        (0, 1536, post) for post in (384, 768, 1536)
    ]
    result = []
    for polynomial in (0x53, 0x5b):
        for mode, timings in (('dac-off', treatments), ('expired-dac-on', controls)):
            for pre, off, post in timings:
                c = Case(
                    f'ch4-{polynomial:02x}-{mode}-p{pre}-d{off}-r{post}',
                    'ch4-dac-phase',
                    'Expire one length count, vary pre-disable/off/post-enable waits, '
                    'then retrigger; compare audio phase with timing-matched DAC-on controls.',
                    parameters={
                        'polynomial': polynomial,
                        'mode': mode,
                        'pre_disable_T': pre,
                        'off_wait_T': off,
                        'post_enable_T': post,
                        'selected_counter_period_T': 1536,
                        'expected_activity': [8, 0, 0, 8],
                        'observable': 'Post-retrigger noise-edge phase, not hidden LFSR evolution.',
                        'timing_caveat': (
                            'P/D/R are wait_cycles arguments, not edge-to-edge intervals. '
                            'P starts after the fixed post-expiry NR52 read and storage. '
                            'Setup/write/read overhead stays fixed: each write_reg is 20 T '
                            'and each NR52 read plus storage is 28 T. DAC-off-to-enable '
                            'writes are D+20 T apart; enable-to-retrigger writes are R+48 T '
                            'apart. DAC-on controls replace EACH NR42 write with a 20 T '
                            'wait, leaving the DAC enabled throughout.'
                        ),
                    },
                )
                # Same fixed setup as the original expired-length CH4 probe.
                c.write(0x25, 0x88)  # Keep NR51 unchanged throughout the case.
                c.write(0x20, 0x3f)  # One length count.
                c.write(0x21, 0xf0)  # Fixed volume 15, envelope pace zero.
                c.write(0x22, polynomial)
                c.write(0x23, 0xc0)
                c.wait(64)
                c.read(0x26, 'NR52-after-initial-trigger')
                c.wait(32768)
                c.read(0x26, 'NR52-after-length-expiry')

                c.wait(pre)
                if mode == 'dac-off':
                    c.write(0x21, 0)
                else:
                    c.wait(20)  # Do not introduce a live NR42/zombie write.
                c.wait(off)
                if mode == 'dac-off':
                    c.write(0x21, 0xf0)
                else:
                    c.wait(20)  # Match the second write independently, too.
                c.wait(post)
                c.read(0x26, 'NR52-before-retrigger')
                c.write(0x23, 0x80)  # Retrigger with length disabled.
                c.wait(64)
                c.read(0x26, 'NR52-after-retrigger')
                c.wait(FRAME)  # At least 65536 T of audio after the final read.
                result.append(c)
    return result
