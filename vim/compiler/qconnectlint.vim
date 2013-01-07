if exists("current_compiler")
	finish
endif

let current_compiler="qconnectlint"

setlocal makeprg=qconnectlint\ %
setlocal errorformat=
	\%f:%l:%c:\ fatal\ %trror:\ %m,
	\%f:%l:%c:\ %trror:\ %m,
	\%f:%l:%c:\ %tarning:\ %m,
	\%f:%l:%c:\ %tonnect:\ %m
