# TSprech 2023/04/13 15:09:20
import cmath

import numpy as np
from PyLTSpice import SimCommander, RawRead


def scale(x, in_min, in_max, out_min, out_max):
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min


def ILModel(L):
    return 0.0055
    # return 0.1817*math.e**(0.0477*L)
    # return L**2 * 0.001


def CRModel(C, fsw):
    # return scale(C, 0.000000001, 0.0001, 20, 0.002)
    return 0.001
    # return 1 / (C * fsw * 6.28)


def calculate_efficiency(individual):
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

    inductance_resistance_model = ILModel
    capacitance_resistance_model = CRModel
    if individual[0] > 0 and individual[1] > 0 and individual[2] > 0:
        fsw = individual[0]
        L = individual[1]
        C = individual[2]
        CCM_Fsw_Min = ((1 - D) * RO) / (2 * L)
        CCM_L_Min = ((1 - D) * RO) / (2 * fsw)

        # Dynamic conditions
        dIL = ((1 - D) * VO) / (L * fsw)  #
        dVC = ((1 - D) * VO) / (8 * L * C * fsw ** 2)  #
        ILmin = (IL - 0.5 * dIL)  #
        ILmax = (IL + 0.5 * dIL)  #

        # Ripple Percentages
        ripple_IL = dIL / IL
        ripple_VC = dVC / VO

        if fsw < CCM_Fsw_Min:
            return 0,

        if L < CCM_L_Min:
            return 0,

        # Typically RMS Current Calculations, but as it is immediately squared after, just avoid the sqrt
        ICrms = cmath.sqrt((1 - D) * (IL ** 2 + (1 / 12) * dIL ** 2) - IO ** 2)
        ILrms = cmath.sqrt((IL ** 2 + (1 / 12) * dIL ** 2))
        INrms = cmath.sqrt(D * (IL ** 2 + (1 / 12) * dIL ** 2))
        IPrms = cmath.sqrt((1 - D) * (IL ** 2 + (1 / 12) * dIL ** 2))

        PCondN = INrms ** 2 * RdsN  #
        PCondP = IPrms ** 2 * RdsP  #
        PCondInd = ILrms ** 2 * inductance_resistance_model(L)  #
        PcondCap = ICrms ** 2 * capacitance_resistance_model(C, fsw)  #

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
        # Prr = PrrP + PrrN  #
        Prr = 0  # GaN
        Pdb = PdbMax + PdbMin  #

        # Power & Efficiency
        PO = VO * IO  #
        PI = VI * II + PCond + Psw + Prr + Pdb  #
        Eff = PO / PI  #

        return Eff * 100,  # THIS COMMA IS EXTREMELY IMPORTANT AND VERY SUBTLE, IT MAKES THE RETURN VALUE A LIST OF 1 WHICH DEAP NEEDS
    return 0,


high_side = True
low_side = False


def frequencytoltpulse(frequency, side: bool):
    deadband = 8  # ns
    period = 1 / (frequency / 1E9)  # Convert to Gigahertz to ensure the period will be in nanoseconds
    if side == low_side:
        return f"PULSE(3.3 0 0n 1n 1n {period + (deadband * 2)}n {period * 2}n)"
    else:
        return f"PULSE(0 3.3 {deadband}n 1n 1n {period}n {period * 2}n)"


def runlt(individual):
    if individual[0] > 0 and individual[1] > 0 and individual[2] > 0:
        fsw = individual[0]
        L = individual[1]
        C = individual[2]

        # select spice model
        LTC = SimCommander("LTFiles/EPC23102_Mine.asc")
        # set default arguments
        LTC.set_component_value('HSin', frequencytoltpulse(fsw, high_side))
        LTC.set_component_value('LSin', frequencytoltpulse(fsw, low_side))

        run_netlist_file = f'F{fsw}_L{L}_C{C}.net'
        LTC.run(run_filename=run_netlist_file)

        LTR = RawRead(run_netlist_file)
        vin = LTR.get_trace('V(VVin)')
        iin = LTR.get_trace('I(VVin)')
        pin = vin * iin

        v_ripple = np.ptp()

