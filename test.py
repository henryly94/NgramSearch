
f = open("1.txt", 'r')

for each in f.readlines():
    print each
    print '=' * 50
    a = each.split(' ')
    for every in a[0]:
        print every, ord(every)
    print '+' * 50

f.close()
