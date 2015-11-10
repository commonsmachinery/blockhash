APPNAME = 'blockhash'
VERSION = '0.2'

top = '.'
out = 'build'

def options(opt):
    opt.load('compiler_c')
    opt.add_option('--debug', dest='debug', action='store_true', default=False, help='Set to debug to enable debug symbols')

def configure(conf):
    conf.load('compiler_c')

    conf.check_cc(lib='m')
    conf.check_cfg(package='MagickWand', args=['--cflags', '--libs'])
    conf.check_cfg(package='opencv', args=['--cflags', '--libs'])

def build(bld):
    bld.stlib(source='blockhash.c', target='stblockhash')
    if bld.options.debug:
       cflags=['-g3', '-ggdb']
    else:
       cflags=['-O3']

    bld.program(source='imagehash.c',
                features='c cprogram',
                target='blockhash',
                use=['MAGICKWAND', 'M', 'stblockhash'],
                cflags=cflags,
               )
    bld.program(source='videohash.c',
                features='c cprogram',
                target='blockhash_video',
                use=['MAGICKWAND', 'M', 'stblockhash', 'OPENCV'],
                cflags=cflags,
               )
