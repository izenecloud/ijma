/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jma;

import org.dom4j.Document;
import org.dom4j.Element;

/**
 *
 * @author jun
 */
public class StatisticRecord {

    private int id_;
    private String rawName_;
    private String diffName_;

    private int upTotal_;
    private int downTotal_;
    private int sameTotal_;

    private int upDiffError_;
    private int downDiffError_;
    private int sameError_;

    public StatisticRecord() {
    }

    public StatisticRecord(int id) {
        id_ = id;
    }
    
    public void setID(int id) {
        id_ = id;
    }

    public int getID() {
        return id_;
    }

    public String getRawName() {
        return rawName_;
    }

    public void setRawName(String fileName) {
        rawName_ = fileName;
    }

    public int getUpTotal() {
        return upTotal_;
    }
    
    public int getDownTotal() {
        return downTotal_;
    }

    public int getSameTotal() {
        return sameTotal_;
    }
    
    public void setTotalValue(int up, int down, int same) {
        upTotal_ = up;
        downTotal_ = down;
        sameTotal_ = same;
    }
    
    public int getUpDiffError() {
        return upDiffError_;
    }
    
    public int getDownDiffError() {
        return downDiffError_;
    }
    
    public int getSameError() {
        return sameError_;
    }

    public void setErrorValue(int upDiff, int downDiff, int same) {
        upDiffError_ = upDiff;
        downDiffError_ = downDiff;
        sameError_ = same;
    }

    public float getUpSamePercent() {
        if(upTotal_ == 0)
            return 0;

        return (float)sameTotal_ / upTotal_ * 100;
    }

    public float getUpAccuracyPercent() {
        if(upTotal_ == 0)
            return 0;

        return (float)(upTotal_ - upDiffError_ - sameError_) / upTotal_ * 100;
    }

    public float getSameAccuracyPercent() {
        if(sameTotal_ == 0)
            return 0;

        return (float)(sameTotal_ - sameError_) / sameTotal_ * 100;
    }

    public float getUpDiffAccuracyPercent() {
        int diffTotal = upTotal_ - sameTotal_;
        if(diffTotal == 0)
            return 0;

        return (float)(diffTotal - upDiffError_) / diffTotal * 100;
    }

    public float getDownSamePercent() {
        if(downTotal_ == 0)
            return 0;

        return (float)sameTotal_ / downTotal_ * 100;
    }

    public float getDownAccuracyPercent() {
        if(downTotal_ == 0)
            return 0;

        return (float)(downTotal_ - downDiffError_ - sameError_) / downTotal_ * 100;
    }

    public float getDownDiffAccuracyPercent() {
        int diffTotal = downTotal_ - sameTotal_;
        if(diffTotal == 0)
            return 0;

        return (float)(diffTotal - downDiffError_) / diffTotal * 100;
    }

    public float getFScore() {
        float sum = upTotal_ + downTotal_;

        if(sum == 0)
            return 0;

        return (float)sameTotal_ * 2 / sum * 100;
    }

    public void setFromDocument(Document diffDoc) {
        Element statElem = diffDoc.getRootElement().element("stat");

        setID(Integer.parseInt(statElem.elementText("id")));
        setRawName(statElem.elementText("name"));

        int upTotal = Integer.parseInt(statElem.elementText("upTotal"));
        int downTotal = Integer.parseInt(statElem.elementText("downTotal"));
        int sameTotal = Integer.parseInt(statElem.elementText("sameTotal"));
        int upDiffError = Integer.parseInt(statElem.elementText("upDiffError"));
        int downDiffError = Integer.parseInt(statElem.elementText("downDiffError"));
        int sameError = Integer.parseInt(statElem.elementText("sameError"));

        setTotalValue(upTotal, downTotal, sameTotal);
        setErrorValue(upDiffError, downDiffError, sameError);
    }
}
