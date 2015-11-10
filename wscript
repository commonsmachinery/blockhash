APPNAME = 'blockhash'
VERSION = '0.2'

top = '.'
out = 'build'

def options(opt):
    opt.load('compiler_c')

def configure(conf):
    conf.load('compiler_c')

    conf.check_cc(lib='m')
    conf.check_cfg(package='MagickWand', args=['--cflags', '--libs'])
    conf.check_cfg(package='opencv', args=['--cflags', '--libs'])

def build(bld):
    bld.stlib(source='blockhash.c', target='stblockhash')

    bld.program(source='main.c',
                features='c cprogram',
                target='blockhash',
                use=['MAGICKWAND', 'M', 'OPENCV', 'stblockhash'],
                cflags=['-O3'],
               )
