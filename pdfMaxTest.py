
from pdfmax import pdfMax

def Test(pdfList):
        print pdfMax(pdfList, 0.1)

        if len(pdfList) > 2:
                for i in range(11):
                        significance = 10.0 ** -i
                        print pdfMax(pdfList, significance)



if __name__ == '__main__':
        Test([(0.123, 0.456)]);

        Test([(1.0,  1.0),
              (0.75, 0.25)]);

        Test([(0.6412027167691333, 0.03163264836021567),
              (0.6281166488299745, 0.013541425967055945),
              (0.6072673296242264, 0.03225156507575445)]);
