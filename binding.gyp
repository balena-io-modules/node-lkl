{
	"targets": [
		{
			"target_name": "bindings",
			"sources": [
				"src/zbindings.cc",
				"src/zsyscall.cc",
				"src/node_lkl.cc"
			],
			'actions': [
				{
					'action_name': "lkl_make",
					'inputs': [
						"src/linux/**"
					],
					'outputs': [
						"src/linux/tools/lkl/liblkl.a"
					],
					'action': [ 'make', '-j4', '-C', 'src/linux/tools/lkl', 'static' ],
					'message': 'Compiling LKL kernel..'
				}
			],
			"cflags_cc": [ '-fpermissive', '-Wno-pointer-arith' ],
			"defines": [ "CONFIG_AUTO_LKL_POSIX_HOST" ],
			"libraries": [
				"../src/linux/tools/lkl/liblkl.a"
			],
			"include_dirs": [
				"<!(node -e \"require('nan')\")",
				"src/linux/tools/lkl/include"
			]
		}
	]
}
