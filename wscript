APPNAME = 'blockhash'
VERSION = '0.3.2'

top = '.'
out = 'build'

def options(opt):
    opt.load('compiler_c')

def configure(conf):
    conf.load('compiler_c')

    conf.check_cc(lib='m')
    conf.check_cfg(package='MagickWand', args=['--cflags', '--libs'])

    if conf.check_cfg(modversion='MagickWand') >= '7':
        conf.define('MAGICKWAND_V7', True)

def build(bld):
    bld(
        features='c cprogram',
        source='blockhash.c',
        target='blockhash',
        use=['MAGICKWAND', 'M'],
        cflags=['-O3', '-DVERSION="' + VERSION + '"'],
    )
