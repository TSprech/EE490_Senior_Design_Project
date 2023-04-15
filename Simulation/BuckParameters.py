# TSprech 2023/04/13 15:09:20
import math

RdsN = 0.0066  # OHMS The on state resistance of the N FET
RdsP = 0.0066  # OHMS The on state resistance of the P FET
VfwP = 0.8  # VOLTS Forward voltage drop of N body diode
VfwN = 0.8  # VOLTS Forward voltage drop of P body diode
TrN = 1.5E-9  # SECONDS Rise time of N FET
TfN = 1.5E-9  # SECONDS Fall time of N FET
TrP = 1.5E-9  # SECONDS Rise time of P FET
TfP = 1.5E-9  # SECONDS Fall time of P FET
QrrN = 5E-64  # COULOMBS Reverse recovery charge of N FET
QrrP = 5E-64  # COULOMBS Reverse recovery charge of P FET

VI = 10  # VOLTS Regulator input voltage
VO = 5  # VOLTS Regulator output voltage
D = VO / VI  # PERCENTAGE Duty cycle required to reach VO
IO = 1  # AMPS Regulator output current
II = D * IO  # AMPS Regulator input current
RO = VO / IO  # OHMS Equivalent output resistance

Tdb = 1 * (1 / 125E6)  # SECONDS Deadband duration
IL = IO  # Only for buck


def ILModel(L):
    return 0.1817*math.e**(0.0477*L)
    # return L**2 * 0.001


def CRModel(C, fsw):
    return 0.01
    # return 1 / (C * fsw * 6.28)


# def calculate_efficiency(fsw, L: float, C):
def calculate_efficiency(individual):
    inductance_resistance_model = ILModel
    capacitance_resistance_model = CRModel
    if individual[0] > 0 and individual[1] > 0 and individual[2] > 0:
        fsw = individual[0]
        L = individual[1]
        C = individual[2]

        # Dynamic conditions
        dIL = ((VI - D * VI) * D) / (L * fsw)  #
        if dIL > 1:
            return 0,
        dVC = ((1 - D) * VO) / (8 * L * C * fsw ** 2)  #

        ILmin = (IL - 0.5 * dIL)  #
        if ILmin < 0:
            return 0,
        ILmax = (IL + 0.5 * dIL)  #
        if ILmax > 2:
            return 0,

        # Typically RMS Current Calculations, but as it is immediately squared after, just avoid the sqrt
        ICrms = (1 - D) * (IL ** 2 + (1 / 12) * dIL ** 2) - IO ** 2
        ILrms = (IL ** 2 + (1 / 12) * dIL ** 2)
        INrms = D * (IL ** 2 + (1 / 12) * dIL ** 2)
        IPrms = (1 - D) * (IL ** 2 + (1 / 12) * dIL ** 2)

        PCondN = INrms * RdsN  #
        PCondP = IPrms * RdsP  #
        PCondInd = ILrms * inductance_resistance_model(L)  #
        PcondCap = ICrms * capacitance_resistance_model(C, fsw)  #

        # Switching, RevRecovery, Deadband Losses
        if ILmin > 0:
            PswPoff = 0  #
            PswNon = 0.5 * VO * ILmin * TrN * fsw  #
            PrrP = 0.5 * VO * QrrP * fsw  #
            PdbMin = ILmin * VfwP * Tdb * fsw  #
        else:
            PswPoff = 0.5 * VO * ILmin * TfP * fsw  #
            PswNon = 0  #
            PrrP = 0  #
            PdbMin = ILmin * VfwN * Tdb * fsw  #

        if ILmax > 0:
            PswNoff = 0.5 * VO * ILmax * TfN * fsw  #
            PswPon = 0  #
            PrrN = 0  #
            PdbMax = ILmax * VfwP * Tdb * fsw  #
        else:
            PswNoff = 0  #
            PswPon = 0.5 * VO * ILmax * TrP * fsw  #
            PrrN = 0.5 * VO * QrrN * fsw  #
            PdbMax = ILmax * VfwN * Tdb * fsw  #

        # Total Power Losses
        PCond = PCondInd + PcondCap + PCondN + PCondP  #
        Psw = PswPon + PswPoff + PswNon + PswNoff  #
        Prr = PrrP + PrrN  #
        Pdb = PdbMax + PdbMin  #

        # Power & Efficiency
        PO = VO * IO  #
        PI = VI * II + PCond + Psw + Prr + Pdb  #
        Eff = PO / PI  #

        # time.sleep(0.1)

        return Eff * 100,  # THIS COMMA IS EXTREMELY IMPORTANT AND VERY SUBTLE, IT MAKES THE RETURN VALUE A LIST OF 1 WHICH DEAP NEEDS
    # time.sleep(0.1)
    return 0,

# print(calculate_efficiency(100E3, 47000*10**-9, 20*10**-6, ILModel, CRModel))