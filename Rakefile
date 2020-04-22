#
# Rakefile for HALO
#
# Copyright(C)2016,2018 Nerry, ALL RIGHTS RESERVED.
#
require 'rake/clean'
require 'rake/packagetask'

ARCH  = ENV['ARCH'] || case `uname -m`
when /i[3456789]86/
  :i386
when /x86.64/
  :x64
when /arm/
  :arm
when /aarch64/
  :aa64
end

PATH_BIN        = "bin/"
PATH_SRC        = "src/"
PATH_OBJ        = "obj/"
PATH_MNT        = "mnt/"
PATH_EFI_BOOT   = "#{PATH_MNT}EFI/BOOT/"
PATH_INC        = "#{PATH_SRC}include/"

case ARCH.to_sym
when :x64
  PATH_OVMF     = "var/bios64.bin"
  QEMU_ARCH     = "x86_64"
  QEMU_OPTS     = ""
when :i386
  PATH_OVMF     = "var/bios32.bin"
  QEMU_ARCH     = "x86_64"
  QEMU_OPTS     = ""
when :arm
  PATH_OVMF     = "var/ovmfarm.fd"
  QEMU_ARCH     = "aarch64"
  QEMU_OPTS     = "-M virt -cpu cortex-a15"
when :aa64
  PATH_OVMF     = "var/ovmfaa64.fd"
  QEMU_ARCH     = "aarch64"
  QEMU_OPTS     = "-M virt -cpu cortex-a57"
else
  raise "UNKNOWN ARCH #{ARCH}"
end

if RUBY_PLATFORM =~ /darwin/ then
  LLVM_PREFIX     = `brew --prefix llvm`.gsub(/\n/, '')
  CC      = ENV['CC'] || "#{LLVM_PREFIX}/bin/clang"
  LD      = ENV['LD'] || "#{LLVM_PREFIX}/bin/lld-link"
else
  CC      = ENV['CC'] || "clang"
  LD      = ENV['LD'] || "lld-link-6.0"
end
CFLAGS  = "-Os -std=c11 -fno-stack-protector -fshort-wchar -mno-red-zone -nostdlibinc -I #{PATH_INC} -Wall -Wpedantic"
AS      = ENV['AS'] || "nasm"
AFLAGS  = "-s -I #{ PATH_SRC }"
LFLAGS  = "-subsystem:efi_application -nodefaultlib -entry:efi_main"

INCS  = [FileList["#{PATH_SRC}*.h"], FileList["#{PATH_INC}*.h"]]

CLEAN.include(FileList["#{PATH_BIN}**/*"])
CLEAN.include(FileList["#{PATH_OBJ}**/*"])

directory PATH_OBJ
directory PATH_BIN
directory PATH_EFI_BOOT

TASKS = [ :halo ]

TASKS.each do |t|
  task t => [t.to_s + ":build"]
end

desc "Defaults"
task :default => [PATH_OBJ, PATH_BIN, TASKS].flatten

desc "Run with QEMU"
task :run => [:default, PATH_EFI_BOOT, PATH_OVMF] do
  (target, efi_suffix) = convert_arch(ARCH)
  FileUtils.cp("#{PATH_BIN}boot#{efi_suffix}.efi", "#{PATH_EFI_BOOT}boot#{efi_suffix}.efi")
  sh "qemu-system-#{QEMU_ARCH} #{QEMU_OPTS} -bios #{PATH_OVMF} -monitor stdio -drive file=fat:ro:mnt"
end

def convert_arch(s)
  case s.to_sym
  when :x64
    ['x86_64-pc-win32-coff', 'x64']
  when :i386
    ['i386-pc-win32-coff', 'ia32']
  when :arm
    ['arm-pc-win32-coff', 'arm']
  when :aa64
    ['aarch64-pc-win32-coff', 'aa64']
  end
end

def make_efi(cputype, target, src_tokens, options = {})

  (cf_target, efi_suffix) = convert_arch(cputype)

  case cputype.to_sym
  when :x64
    af_target = "-f win64"
  when :i386
    af_target = "-f win32"
  end

  path_obj      = "#{PATH_OBJ}#{efi_suffix}/"
  directory path_obj

  if options[:base]
    path_src_p    = "#{PATH_SRC}#{options[:base]}/"
  else
    path_src_p    = "#{PATH_SRC}"
  end

  if options[:no_suffix]
    efi_output    = "#{PATH_BIN}#{target}.efi"
  else
    efi_output    = "#{PATH_BIN}#{target}#{efi_suffix}.efi"
  end

  srcs = src_tokens.map do |s|
    if s !~ /\.\w+/
      s += '.c'
    end
    base = File.basename(s, '.*')
    ext = File.extname(s)
    if s !~ /\//
      s = [
        "#{path_src_p}#{s}",
        "#{path_src_p}#{base}-#{efi_suffix}#{ext}",
        "#{PATH_SRC}#{s}",
        "#{PATH_SRC}#{base}-#{efi_suffix}#{ext}",
      ].find do |q|
        if File.exist?(q)
          q
        end
      end
    end
    s
  end

  objs = srcs.map do |src|
    mod_name = File.basename(src, '.*')
    obj = "#{path_obj}#{mod_name}.o"

    case File.extname(src)
    when '.c'
      file obj => [ src, INCS, path_obj ].flatten do |t|
        sh "#{ CC } -target #{ cf_target } #{ CFLAGS} -c -o #{ t.name } #{ src }"
      end
    when '.asm'
      file obj => [ src, path_obj ] do | t |
        sh "#{ AS } #{ af_target } #{ AFLAGS } -o #{ t.name } #{ src }"
      end
    end

    obj
  end

  file efi_output => objs do |t|
    sh "#{LD} #{ LFLAGS} #{ t.prerequisites.join(' ') } -out:#{ t.name }"
  end

  efi_output
end


namespace :halo do

  targets = []

  %w(x64 i386).each do |arch|
    targets << make_efi(arch, 'boot', %w( halo tek haloasm.asm ))
  end

  desc "Build HALO"
  task :build => targets

end
