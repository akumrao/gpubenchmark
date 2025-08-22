/*
 * Java class for Benchmark List
 * Also it stores the test case results path
 */



package org.gpu.glload;

import java.util.ArrayList;
import java.util.Collections;
import java.io.*;

import android.app.Activity;
import android.os.Environment;

class BenchmarkListManager {

    private ArrayList<String> benchmarks;
    private Activity activity;

    BenchmarkListManager(Activity activity, ArrayList<String> benchmarks)
    {
        this.activity = activity;
        if (benchmarks == null) {
            this.benchmarks = new ArrayList<String>();
            this.benchmarks.add("Add Load...");
        }
        else {
            this.benchmarks = benchmarks;
        }
    }

    /** 
     * Gets the list holding the benchmarks.
     *
     * The reference to this list is constant for the life of
     * the BenchmarkListManager,
     * 
     * @return the operation error code
     */
    ArrayList<String> getBenchmarkList() {
        return benchmarks;
    }

    /** 
     * Gets the saved benchmark lists.
     * 
     * Each list name is prefixed with either "internal/" or "external/"
     * to denote in which storage area it is saved in.
     * 
     * @return an array containing the saved list names
     */
     String[] getSavedLists() {
        File externalPath = getSavedListPath(true);
        File internalPath = getSavedListPath(false);
        ArrayList<String> lists = new ArrayList<String>();

        if (externalPath != null && externalPath.isDirectory()) {
            for (File f: externalPath.listFiles())
                lists.add("external/" + f.getName());
        }

        if (internalPath != null && internalPath.isDirectory()) {
            for (File f: internalPath.listFiles())
                lists.add("internal/" + f.getName());
        }

        Collections.sort(lists);

        String[] a = new String[0];
        return lists.toArray(a);
    }

    /** 
     * Saves the current benchmark list to a file.
     * 
     * @param listName the list filename
     * @param external whether the file is to be stored in external storage
     */
    void saveBenchmarkList(String listName, boolean external) throws Exception {
        File listPath = getSavedListPath(external);
        if (listPath == null)
            throw new Exception("External storage not present");

        listPath.mkdirs();

        File f = new File(listPath, listName);

        BufferedWriter out = new BufferedWriter(new FileWriter(f));
        try {
            for (int i = 0; i < benchmarks.size() - 1; i++) {
                out.write(benchmarks.get(i));
                out.newLine();
            }
        }
        catch (Exception ex) {
            throw ex;
        }
        finally {
            out.close();
        }
    }

    /** 
     * Loads a benchmark list from a file.
     * 
     * @param listName the list filename
     * @param external whether the file is stored in external storage
     */
    void loadBenchmarkList(String listName, boolean external) throws Exception {
        /* Get the list file path */
        File listPath = getSavedListPath(external);
        if (listPath == null)
            throw new Exception("External storage not present");

        File f = new File(listPath, listName);

        ArrayList<String> newBenchmarks = new ArrayList<String>();

        /* Read benchmarks from file */
        BufferedReader reader = new BufferedReader(new FileReader(f));
        String line = null;

        while ((line = reader.readLine()) != null)
            newBenchmarks.add(line);

        /* If everything went well, replace current benchmarks */
        benchmarks.clear();
        benchmarks.addAll(newBenchmarks);
        benchmarks.add("Add benchmark...");
    }

    /** 
     * Delete a benchmark list file.
     * 
     * @param listName the list filename
     * @param external whether the file is stored in external storage
     */
    void deleteBenchmarkList(String listName, boolean external) throws Exception {
        /* Get the list file path */
        File listPath = getSavedListPath(external);
        if (listPath == null)
            throw new Exception("External storage not present");

        File f = new File(listPath, listName);
        f.delete();
    }

    /** 
     * Gets the path where benchmark lists are saved in.
     * 
     * @param external whether to get the path for external storage
     * 
     * @return the saved list path
     */
     File getSavedListPath(boolean external) {
        File f = null;

        if (external) {
            String state = Environment.getExternalStorageState();
            if (!Environment.MEDIA_MOUNTED.equals(state))
                return null;
            f = activity.getExternalFilesDir(null);
        }
        else {
            f = activity.getExternalFilesDir(null);
        }

        if (f != null)
            f = new File(f, "lists");

        return f;
    }
}
