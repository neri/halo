# -*- coding: utf-8 -*-
#
# Rakefile for HALO
#
# Copyright(C)2016 Nerry, ALL RIGHTS RESERVED.
#
require 'rake/clean'
require 'rake/packagetask'
#require 'msgpack'

case `uname -m`
when /i[3456789]86/
  ARCH = :x86
when /x86.64/
  ARCH = :x64
when /aarch64/
  ARCH = :aa64
else
  ARCH = :unknown
end

PATH_OUTPUT     = "output/"
PATH_BIN        = "bin/"
PATH_SRC        = "src/"
PATH_TOOLS      = "tools/"
PATH_MISC       = "misc/"
PATH_OBJ        = "obj/"

EFI_CC  = ENV['EFI_CC'] || ENV['CC'] || "cc"
CFLAGS  = "-Os"
EFI_AS   = ENV['AS'] || "nasm"
AFLAGS  = "-s -I #{ PATH_SRC } -f bin"
EFI_LD  = ENV['EFI_LD'] || ENV['LD'] || "ld"
OBJCOPY = ENV['OBJCOPY'] || "objcopy"
EFI_INC = ENV['EFI_INC'] || "/usr/include/efi"
EFI_LIB = ENV['EFI_LIB'] || "/usr/lib"

INCS  = FileList["#{PATH_SRC}*.h"]

CLEAN.include(FileList["#{PATH_OUTPUT}**/*"])
CLEAN.include(FileList["#{PATH_BIN}**/*"])
CLEAN.include(FileList["#{PATH_OBJ}**/*"])

directory PATH_OUTPUT
directory PATH_OBJ
directory PATH_BIN

TASKS = [ :tools, :halo ]

TASKS.each do |t|
  task t => [t.to_s + ":build"]
end

desc "Defaults"
task :default => [PATH_OUTPUT, PATH_OBJ, PATH_BIN, TASKS].flatten


namespace :tools do

  targets = []

  desc "Build Tools"
  task :build => [targets].flatten

end


namespace :halo do

  def make_efi(cputype, target, src_tokens, options = {})
    case cputype.to_sym
    when :x64
      efi_arch = 'x86_64'
      efi_suffix = 'x64'
    when :x86
      efi_arch = 'ia32'
      efi_suffix = 'ia32'
    else
      efi_arch = cputype.to_s
      efi_suffix = efi_arch
    end

    path_obj      = "#{PATH_OBJ}#{efi_suffix}/"
    directory path_obj

    efi_includes  = "-I#{EFI_INC} -I#{EFI_INC}/#{efi_arch} -I#{EFI_INC}/protocol"
    efi_crt_obj   = "#{EFI_LIB}/crt0-efi-#{efi_arch}.o"
    efi_lds       = "#{EFI_LIB}/elf_#{efi_arch}_efi.lds"
    efi_cflags    = "-O3 #{efi_includes} -std=c99 -fno-stack-protector -fpic -fshort-wchar -mno-red-zone"
    efi_ldflags   = "-nostdlib -znocombreloc -T #{efi_lds} -shared -Bsymbolic -L #{EFI_LIB} #{efi_crt_obj}"

    case efi_arch
    when 'x86_64'
      efi_cflags << " -m64 -DEFI_FUNCTION_WRAPPER"
      efi_aflags      = "-f elf64"

    when 'ia32'
      efi_cflags << " -m32 -march=i686"

    end

    efi_obj_so    = "#{path_obj}#{target}.so"
    if options[:no_suffix]
      efi_output    = "#{PATH_BIN}#{target}.efi"
    else
      efi_output    = "#{PATH_BIN}#{target}#{efi_suffix}.efi"
    end

    srcs = src_tokens.map do |s|
      if s !~ /\.\w+/
        s += '.c'
      end
      if s !~ /\//
        s = "#{PATH_SRC}#{s}"
      end
      s
    end

#    path_deps     = "#{path_obj}#{target}-dependencies.json"

    objs = srcs.map do |src|
      mod_name = File.basename(src, '.*')
      obj = "#{path_obj}#{mod_name}.o"

      case File.extname(src)
      when '.c'
        file obj => [ src, INCS, path_obj ].flatten do |t|
          sh "#{ EFI_CC } #{efi_cflags} -c -o #{ t.name } #{ src }"
        end
      when '.asm'
        file obj => [ src, path_obj ] do | t |
          sh "#{ EFI_AS } #{ efi_aflags } -o #{ t.name } #{ src }"
        end
      end

      obj
    end

    file efi_obj_so => objs do |t|
      sh "#{EFI_LD} #{efi_ldflags} #{ t.prerequisites.join(' ') } -o #{ t.name } -lefi -lgnuefi"
    end

    file efi_output => efi_obj_so do |t|
      sh "#{OBJCOPY} --strip-all -j .text -j .sdata -j .data -j .dynamic -j .dynsym  -j .rel -j .rela -j .reloc --target=efi-app-#{efi_arch} #{ t.prerequisites[0] } #{ t.name }"
    end

    efi_output
  end


  %w(x64).each do |arch|
    dependencies = make_efi(arch, 'boot', %w( halo tek haloasm.asm ))
    desc "Build HALO"
    task :build => dependencies
  end

end
