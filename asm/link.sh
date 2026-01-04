awk '{
  print
  if(!done && $0 ~ /^main:([[:space:]]*$|[[:space:]]+)/) {
    print "1:"
    print "auipc gp, %pcrel_hi(__global_pointer$)"
    print "addi  gp, gp, %pcrel_lo(1b)"
    done=1
  }
}' main.s > main.s.new && mv main.s.new main.s
