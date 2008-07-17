//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

const CONGRATULATIONS_DE = [
    "N" + UE_LOWER + "chtern bilanziert: Ihre Anlagestrategie sollte dringend verbessert werden. Zwar h" + AE_LOWER + "lt sich Ihr Verlust in Grenzen, und Sie haben versucht Risiken zu minimieren, aber Sie haben auch einige gute Chancen verpasst. Versuchen Sie es noch einmal und lassen Sie sich diesmal nicht mehr aufs Glatteis f" + UE_LOWER + "hren.",
    "Sie haben ihr Kapital gehalten und im Lauf des Spiels weder dazu gewonnen noch verloren. An der echten B" + OE_LOWER + "rse h" + AE_LOWER + "tten Sie streng genommen ihre Zeit vertan. Denn Ihre Devise sollte \"Rendite und Kursgewinn\" lauten! Spielen Sie noch eine Runde und versuchen Sie diesmal, Ihre Aktienwerte zu steigern.",
    "Gratulation! Sie haben ihr Kapital mit einer durchdachten Anlagestrategie stetig vermehrt. Ihre Rendite kann sich sehen lassen!",
    "Sensationell! Ihre Aktien haben die maximale Rendite erwirtschaftet und Sie haben dabei souver" + AE_LOWER + "n und entschlossen gehandelt. Bei unserem Aktienspiel geh" + OE_LOWER + "ren sie zu den Besten!"
];

const CONGRATULATIONS_LOTTERY_DE = [
    null,
    null,
    "Spielen Sie noch eine weitere Runde, vielleicht knacken Sie den High Score, kommen ins Investor Relations Ranking und gewinnen eine Fahrt an die Frankfurter B" + OE_LOWER + "rse.",
    "Als B" + OE_LOWER + "rsentalent werden Sie daher ins Ranking aufgenommen und haben mit etwas Gl" + UE_LOWER + "ck die Chance, eine Fahrt an die B" + OE_LOWER + "rse in Frankfurt am Main zu gewinnen."
];

const CONGRATULATIONS_EN = [
    "A sober assessment: your investment strategies need to improve urgently. Although you have kept your losses within certain parameters and have tried to minimise risks, you have missed some great opportunities. Please try again and try to avoid taking excessive risks this time.",
    "You have retained your capital but have shown neither gains nor losses during the course of the game. If you had been playing the real stock exchange you would have gained nothing and wasted a good deal of time. Your outcome should have read \"return and capital gains\"! Please play again and perhaps this time you will be more successful in improving your share prices.",
    "Congratulations! Your carefully thought-out strategy has yielded results and you have steadily increased your capital. The return on your investment is something to be proud of!",
    "Sensational! You have traded your shares at a maximum return and your actions have been assessed as decisive and confident. The game you have played shows yours to be an outstanding achievement!"
];

const CONGRATULATIONS_LOTTERY_EN = [
    null,
    null,
    "Why not play another round, perhaps you will crack the \"high score\", attain a place in our top traders and win a trip to the Frankfurt Stock Exchange.",
    "Because of your talent at playing the stock exchange you have won a place in our exclusive group of traders and now have a chance of winning a trip to the Frankfurt Stock Exchange."
];

const CONGRATULATIONS = {
    en:CONGRATULATIONS_EN,
    de:CONGRATULATIONS_DE
};

const CONGRATULATIONS_LOTTERY = {
    en:CONGRATULATIONS_LOTTERY_EN,
    de:CONGRATULATIONS_LOTTERY_DE
};

function getCongratulationText(theLanguage, theScore, theChanceForWinning, theIs2008) {
    var myCongratulations = CONGRATULATIONS[theLanguage];
    var myText = "";

    switch(theLanguage) {
    case "de":
        myText += "Sie haben " + theScore.toFixed(2) + EURO_SIGN + " auf dem Konto.";
        break;
    case "en":
        myText += "Your account balance is " + theScore.toFixed(2) + EURO_SIGN + ".";
        break;
    }

    myText += "\n";

    var myRating = 0;

    if(theScore < LOWER_CREDIT) {
        myRating = 0;
    } else if (theScore <= MID_CREDIT) {
        myRating = 1;
    } else if (theScore <= UPPER_CREDIT) {
        myRating = 2;
    } else {
        myRating = 3;
    }

    myText += myCongratulations[myRating];

    if(theChanceForWinning && theIs2008) {
        var myLotteryCongratulations = CONGRATULATIONS_LOTTERY[theLanguage];
        if(myRating in myLotteryCongratulations) {
            myText += " " + myLotteryCongratulations[myRating];
        }

        myText += "\n";
        
        switch(theLanguage) {
        case "de":
            myText += "M" + OE_LOWER + "chten Sie am Gewinnspiel teilnehmen?";
            break;
        case "en":
            myText += "Would you like to take part in the competition?";
            break;
        }
    }

    return myText;
}

// XXX: these texts are NOT final
function getIntroductionText(theLanguage) {
    var myText = "";

    switch(theLanguage) {
    case "de":
        myText += "Achten Sie auf die Nachrichten und investieren Sie Ihr Kapital in eines der Unternehmen.\nVerpassen Sie keine Chance und wechseln Sie Ihre Investitionen so oft Sie m" + OE_LOWER + "chten. Bei schlechtem B" + OE_LOWER + "rsenwetter parken Sie Ihr Geld auf dem " + EURO_SIGN + "-Konto. Machen Sie innerhalb von\n3 Minuten soviel Gewinn wie m" + OE_LOWER + "glich.";
        break;
    case "en":
        myText += "Pay attention to Stock Exchange related news and invest your capital in one of the following companies. Don't loose any opportunities and change your investment as often as you like. When the markets are unstable park your money on the " + EURO_SIGN + "-account. See how much money you can make in the space of 3 minutes.";
        break;
    }

    return myText;
}


var ourStaticTextNode = new Node();
ourStaticTextNode.parseFile("CONFIG/statictext.xml");
ourStaticTextNode = ourStaticTextNode.firstChild;

function getInternationalizedText(theLanguage, theId) {
    var myNode = getDescendantByName(ourStaticTextNode, theId);
    if (myNode) {
        if (theLanguage in myNode) {
            return myNode[theLanguage];
        } else {
            Logger.error("Static text '" + theId + "' not found for language '" + theLanguage + "'.");
            return "";
        }
    }

    Logger.error("No static text entry for '" + theId + "' found.");
    return "";   
}
